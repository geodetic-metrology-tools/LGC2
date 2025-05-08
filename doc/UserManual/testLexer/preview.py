from pygments import highlight
from pygments.formatters import HtmlFormatter
from lgc_lexer import LgcLexer

code = open("test.lgc").read()
formatter = HtmlFormatter(full=True, style="default")
html = highlight(code, LgcLexer(), formatter)

with open("test.html", "w") as f:
    f.write(html)

print("✅ HTML generated! Open test.html to preview.")