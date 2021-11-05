import os
import sys
import subprocess


if "DOXYREST_PATH" in os.environ:
    DOXYREST_PATH = os.environ["DOXYREST_PATH"]
    sys.path.append(os.path.join(DOXYREST_PATH, 'sphinx'))


def run_doxygen(folder):
    """Run the doxygen make command in the designated folder"""

    try:
        retcode = subprocess.call("cd %s; cmake .. ; make doc " % folder, shell=True)
        if retcode < 0:
            sys.stderr.write("doxygen terminated by signal %s" % (-retcode))
    except OSError as e:
        sys.stderr.write("doxygen execution failed: %s" % e)


def generate_doxygen_xml(app):
    """Run the doxygen make commands if we're on the ReadTheDocs server"""

    read_the_docs_build = os.environ.get('READTHEDOCS', None) == 'True'

    if read_the_docs_build:
        run_doxygen(".")

def setup(app):
    # Add hook for building doxygen xml when needed
    app.connect("builder-inited", generate_doxygen_xml)


project = 'godzilla'
copyright = '2021, David Andrs'
author = 'David Andrs'

# extensions = [
#     'doxyrest',
#     'cpplexer'
# ]

templates_path = ['_templates']

exclude_patterns = []

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
