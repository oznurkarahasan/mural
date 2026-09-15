import os
import shutil

Import("env")

print("Transpiling TS code")
worker_dir = "data/www/worker"
if os.path.exists(worker_dir):
    shutil.rmtree(worker_dir, ignore_errors=True)

currentPath = os.getcwd()

os.chdir('./tsc')
env.Execute("npm run build")
if not os.path.exists("../data/www/worker/"):
    os.makedirs("../data/www/worker/")
shutil.copy("dist_packed/main.js", "../data/www/worker/worker.js")
os.chdir(currentPath)