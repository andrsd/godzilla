import os
import sys
import subprocess


if "DOXYREST_PATH" in os.environ:
    DOXYREST_PATH = os.environ["DOXYREST_PATH"]
    sys.path.append(os.path.join(DOXYREST_PATH, 'sphinx'))


def build_devel(folder):
    """Build the devel section of the doco"""

    try:
        retcode = subprocess.call("cmake .. -DPROJECT_RST_DIR=.", shell=True)
        retcode = subprocess.call("doxygen Doxyfile.doxygen", shell=True)
        retcode = subprocess.call("doxyrest -c doxyrest-config.lua", shell=True)
        if retcode < 0:
            sys.stderr.write("Terminated by signal %s" % (-retcode))
    except OSError as e:
        sys.stderr.write("Execution failed: %s" % e)

def generate_devel_rst(app):
    # we build in `/docs` dir so we don't have to worry about moving the rest of the .rst files
    build_devel(".")

def setup(app):
    read_the_docs_build = os.environ.get('READTHEDOCS', None) == 'True'
    if read_the_docs_build:
        app.connect("builder-inited", generate_devel_rst)


project = 'godzilla'
copyright = '2021, David Andrs'
author = 'David Andrs'

extensions = [
    'doxyrest',
    'cpplexer',
    'sphinx_tabs.tabs'
]

templates_path = ['_templates']

exclude_patterns = []

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
html_show_sourcelink = False
