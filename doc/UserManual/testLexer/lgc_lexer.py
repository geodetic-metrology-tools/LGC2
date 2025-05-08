# lgc_lexer.py
from pygments.lexer import RegexLexer
from pygments.token import Text, Keyword, Number, Name, Comment

class LgcLexer(RegexLexer):
    name = 'LGC'
    aliases = ['lgc']
    filenames = ['*.lgc']

    tokens = {
        'root': [
            (r'\*.*', Keyword),             # Lines starting with *
            (r'\b\d+\.\d+\b', Number.Float),
            (r'\b\d+\b', Number.Integer),
            (r'[A-Z_]+', Name.Constant),    # UPPERCASE names
            (r'\s+', Text),
            (r'%.*', Comment.Single),       # Comments with %
        ]
    }
