virtualenv --python=python3.7 venv
source venv/bin/activate
pip install -r requirements.txt

jupyter notebook --generate-config -y
echo 'c.NotebookApp.contents_manager_class = "jupytext.TextFileContentsManager"' >> ~/.jupyter/jupyter_notebook_config.py
