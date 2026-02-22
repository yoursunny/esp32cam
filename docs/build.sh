#!/bin/bash
set -euo pipefail
cd "$( dirname "${BASH_SOURCE[0]}" )"

BASEURI=https://esp32cam.yoursunny.dev
NS=esp32cam

doxygen Doxyfile 2>&1 | ./filter-Doxygen-warning.awk 1>&2

find html -maxdepth 1 -name '*.html' | while read -r F; do
    PAGENAME=$(basename "$F")
    if [[ $PAGENAME == index.html ]]; then
        PAGENAME=
    fi
    CANONICAL="<link rel=\"canonical\" href=\"$BASEURI/$PAGENAME\">"
    ANALYTICS='<script async src="https://www.googletagmanager.com/gtag/js?id=G-MLZ5G2C4X2"></script><script>window.dataLayer=[];function gtag(){dataLayer.push(arguments);}if(location.hostname.endsWith(".yoursunny.dev")){gtag("js",new Date());gtag("config","G-MLZ5G2C4X2");}</script>'
    sed -i "/<\/head>/ i $CANONICAL$ANALYTICS" "$F"
done

SITEMAP=html/sitemap.xml
echo '<?xml version="1.0" encoding="UTF-8"?>' >$SITEMAP
echo '<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">' >>$SITEMAP
find html -maxdepth 1 \
    \( -name 'index.html' -o -name 'annotated.html' -o -name "namespace$NS*.html" -o -name "class$NS*.html" -o -name "struct$NS*.html" \) \
    ! -name '*-members.html' | while read -r F; do
    PAGENAME=$(basename "$F")
    if [[ $PAGENAME == index.html ]]; then
        PAGENAME=
    fi
    echo "<url><loc>$BASEURI/$PAGENAME</loc></url>"
done >>$SITEMAP
echo '</urlset>' >>$SITEMAP

echo -e "User-Agent: *\nAllow: /\nSitemap: $BASEURI/sitemap.xml" >html/robots.txt

cp _redirects html/
