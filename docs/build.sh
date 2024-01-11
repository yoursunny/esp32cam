#!/bin/bash
set -euo pipefail
cd "$( dirname "${BASH_SOURCE[0]}" )"

doxygen Doxyfile 2>&1 | ./filter-Doxygen-warning.awk 1>&2

find html -name '*.html' | xargs sed -i '/<\/head>/ i\<script async src="https://www.googletagmanager.com/gtag/js?id=G-MLZ5G2C4X2"></script><script>window.dataLayer=[];function gtag(){dataLayer.push(arguments);}if(location.hostname.endsWith(".yoursunny.dev")){gtag("js",new Date());gtag("config","G-MLZ5G2C4X2");}</script>'
cp _redirects html/
