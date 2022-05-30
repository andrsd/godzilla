import os
import sys
import subprocess


read_the_docs_build = os.environ.get('READTHEDOCS', None) == 'True'
if read_the_docs_build:
    # Wish this was set by readthedocs
    DOXYREST_PATH = "/home/docs/checkouts/readthedocs.org/user_builds/{}/conda/{}/share/doxyrest".format(
        os.environ["READTHEDOCS_PROJECT"],
        os.environ["READTHEDOCS_VERSION"])
    sys.path.append(os.path.join(DOXYREST_PATH, 'sphinx'))
    os.environ["DOXYREST_PATH"] = DOXYREST_PATH
elif "DOXYREST_PATH" in os.environ:
    sys.path.append(os.path.join(os.environ["DOXYREST_PATH"], 'sphinx'))


def build_devel(folder):
    """Build the devel section of the doco"""

    try:
        retcode = subprocess.call("cmake .. -DGODZILLA_RST_DIR={} -DDOXYREST_PATH={}".format(folder, DOXYREST_PATH), shell=True)
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

html_theme = 'sphinx_material'
html_theme_options = {
    'nav_title': 'GODZILLA',
    'base_url': 'https://godzilla-fem.readthedocs.io',
    'color_primary': 'indigo',
    'color_accent': 'indigo',
    'repo_url': 'https://github.com/andrsd/godzilla',
    'repo_name': 'godzilla',
    'globaltoc_depth': 3,
    'master_doc': False
}
html_sidebars = {
    "**": ["logo-text.html", "globaltoc.html", "localtoc.html", "searchbox.html"]
}
html_static_path = ['_static']
html_show_sourcelink = False
