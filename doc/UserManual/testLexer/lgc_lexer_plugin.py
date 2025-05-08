from mkdocs.plugins import BasePlugin
from pygments.lexers import _mapping
from pygments import lexers
from lgc_lexer import LgcLexer

class LgcLexerPlugin(BasePlugin):
    def on_config(self, config):
        print("✅ LgcLexerPlugin loaded")

        # Add to Pygments manually
        lexers.LEXERS['LgcLexer'] = (
            'lgc_lexer',
            'LGC',
            ('lgc',),
            ('*.lgc',),
            ('text/x-lgc',)
        )

        # Ensure cache is cleared and picked up
        lexers._lexer_cache.clear()
        return config
