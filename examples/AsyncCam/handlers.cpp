#include "AsyncCam.hpp"
#include <StreamString.h>

static const char FRONTPAGE[] = R"EOT(
<!doctype html>
<title>esp32cam AsyncCam example</title>
<body>
<h1>esp32cam AsyncCam example</h1>
<form id="update"><p>
<select name="resolution" required>%resolution%</select>
%brightness%
%contrast%
%saturation%
%hmirror%
%vflip%
<input type="submit" value="update">
</p></form>
<p id="controls">
<button data-act="mjpeg">show Motion JPEG stream</button>
<button data-act="jpg">show still JPEG image</button>
<button data-act="">hide</button>
</p>
<div id="display"></div>
<footer>Powered by <a href="https://esp32cam.yoursunny.dev/">esp32cam</a></footer>
<script type="module">
async function fetchText(uri, init) {
  const response = await fetch(uri, init);
  if (!response.ok) {
    throw new Error(await response.text());
  }
  return (await response.text()).trim().replaceAll("\r\n", "\n");
}

const $update = document.querySelector("#update");
const $display = document.querySelector("#display");
$update.addEventListener("submit", async (evt) => {
  evt.preventDefault();
  try {
    await fetchText("/update.cgi", {
      method: "POST",
      body: new URLSearchParams(new FormData($update)),
    });
  } catch (err) {
    $display.textContent = err.toString();
  }
});
$update.reset();

for (const $ctrl of document.querySelectorAll("#controls button")) {
  $ctrl.addEventListener("click", (evt) => {
    evt.preventDefault();

    const $img = $display.querySelector("img");
    if ($img) {
      $img.src = "";
    }

    const act = evt.target.getAttribute("data-act");
    if (act === "") {
      $display.innerHTML = "";
    } else {
      $display.innerHTML = `<img src="/cam.${act}?_=${Math.random()}" alt="camera image">`;
    }
  });
}
</script>
)EOT";

static String
rewriteFrontpage(const esp32cam::Settings& s, const String& var) {
  StreamString b;

  if (var == "resolution") {
    for (const auto& r : esp32cam::Camera.listResolutions()) {
      b.printf("<option value=\"%d\"%s>", r.as<int>(),
               r > initialResolution ? " disabled"
               : r == s.resolution   ? " selected"
                                     : "");
      b.print(r);
      b.print("</option>");
    }
  }

#define SETTING_INT(MEM, MIN, MAX)                                                                 \
  else if (var == #MEM) {                                                                          \
    b.printf("<label>" #MEM "=<input type=\"number\" name=\"" #MEM                                 \
             "\" value=\"%d\" min=\"%d\" max=\"%d\"></label>",                                     \
             s.MEM, MIN, MAX);                                                                     \
  }

#define SETTING_BOOL(MEM)                                                                          \
  else if (var == #MEM) {                                                                          \
    b.printf("<label><input type=\"checkbox\" name=\"" #MEM "\" value=\"1\"%s>" #MEM "</label>",   \
             s.MEM ? " checked" : "");                                                             \
  }

  SETTING_INT(brightness, -2, 2)
  SETTING_INT(contrast, -2, 2)
  SETTING_INT(saturation, -2, 2)
  SETTING_BOOL(hmirror)
  SETTING_BOOL(vflip)

#undef SETTING_INT
#undef SETTING_BOOL

  return b;
}

static void
handleFrontpage(AsyncWebServerRequest* req) {
  auto settings = esp32cam::Camera.status();
  req->send(200, "text/html", reinterpret_cast<const uint8_t*>(FRONTPAGE), sizeof(FRONTPAGE),
            [=](const String& var) { return rewriteFrontpage(settings, var); });
}

static void
handleUpdate(AsyncWebServerRequest* req) {
  bool ok = esp32cam::Camera.update([=](esp32cam::Settings& s) {
#define UPDATE(MEM)                                                                                \
  do {                                                                                             \
    if constexpr (std::is_same_v<decltype(s.MEM), bool>) {                                         \
      s.MEM = req->hasArg(#MEM);                                                                   \
    } else {                                                                                       \
      s.MEM = static_cast<decltype(s.MEM)>(req->arg(#MEM).toInt());                                \
    }                                                                                              \
  } while (false)
    s.resolution = esp32cam::Resolution(static_cast<int>(req->arg("resolution").toInt()));
    UPDATE(brightness);
    UPDATE(contrast);
    UPDATE(saturation);
    UPDATE(hmirror);
    UPDATE(vflip);

#undef UPDATE
  });

  if (!ok) {
    req->send(500, "text/plain", "update settings error\n");
    return;
  }
  req->send(204);
}

void
addRequestHandlers() {
  server.on("/robots.txt", HTTP_GET, [](AsyncWebServerRequest* req) {
    req->send(200, "text/plain", "User-Agent: *\nDisallow: /\n");
  });

  server.on("/", HTTP_GET, handleFrontpage);
  server.on("/update.cgi", HTTP_POST, handleUpdate);

  server.on("/cam.jpg", esp32cam::asyncweb::handleStill);
  server.on("/cam.mjpeg", esp32cam::asyncweb::handleMjpeg);
}
