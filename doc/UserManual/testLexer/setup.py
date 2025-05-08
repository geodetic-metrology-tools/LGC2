from setuptools import setup

setup(
    name='mkdocs-lgc-lexer-plugin',
    version='0.1',
    py_modules=['lgc_lexer_plugin', 'lgc_lexer'],
    entry_points={
        'mkdocs.plugins': [
            'lgcLexerPlugin = lgc_lexer_plugin:LgcLexerPlugin'
        ]
    },
    install_requires=[
        'mkdocs',
        'pygments'
    ],
)
