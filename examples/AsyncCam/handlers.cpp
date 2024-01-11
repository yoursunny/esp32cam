#include "AsyncCam.hpp"
#include <StreamString.h>

static const char FRONTPAGE[] = R"EOT(
<!doctype html>
<title>esp32cam AsyncCam example</title>
<body>
<h1>esp32cam AsyncCam example</h1>
<form><p>
Resolution
<select id="resolution" required>%RESOLUTION_OPTIONS%</select>
<input type="submit" value="change">
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

const $display = document.querySelector("#display");
const $resolution = document.querySelector("#resolution");

$resolution.form.addEventListener("submit", async (evt) => {
  evt.preventDefault();
  const [width, height] = $resolution.value.split("x");
  try {
    const change = await fetchText("/change-resolution.cgi", {
      method: "POST",
      body: new URLSearchParams({ width, height }),
    });
  } catch (err) {
    $display.textContent = err.toString();
  }
});

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
rewriteFrontpage(const String& var) {
  StreamString b;
  if (var == "RESOLUTION_OPTIONS") {
    for (const auto& r : esp32cam::Camera.listResolutions()) {
      b.print("<option");
      if (r == currentResolution) {
        b.print(" selected");
      }
      if (r > initialResolution) {
        b.print(" disabled");
      }
      b.print('>');
      b.print(r);
      b.print("</option>");
    }
  }
  return b;
}

void
addRequestHandlers() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/html", reinterpret_cast<const uint8_t*>(FRONTPAGE),
                    sizeof(FRONTPAGE), rewriteFrontpage);
  });

  server.on("/robots.txt", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/html", "User-Agent: *\nDisallow: /\n");
  });

  server.on("/change-resolution.cgi", HTTP_POST, [](AsyncWebServerRequest* request) {
    long width = request->arg("width").toInt();
    long height = request->arg("height").toInt();
    auto r = esp32cam::Camera.listResolutions().find(width, height);
    if (!(r.isValid() && r.getWidth() == width && r.getHeight() == height)) {
      request->send(404, "text/plain", "non-existent resolution\n");
      return;
    }

    if (!esp32cam::Camera.changeResolution(r, 0)) {
      Serial.printf("changeResolution(%ld,%ld) failure\n", width, height);
      request->send(500, "text/plain", "changeResolution error\n");
      return;
    }

    currentResolution = r;
    Serial.printf("changeResolution(%ld,%ld) success\n", width, height);
    StreamString b;
    b.print(currentResolution);
    request->send(b, "text/plain", b.length());
  });

  server.on("/cam.jpg", esp32cam::asyncweb::handleStill);
  server.on("/cam.mjpeg", esp32cam::asyncweb::handleMjpeg);
}
