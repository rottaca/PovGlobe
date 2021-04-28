import flask
from flask import request
import threading

from globe_wrapper import GlobeWrapper

app = flask.Flask(__name__)

globe_wrapper = GlobeWrapper()
sem = threading.Semaphore()


@app.route("/", methods=["GET"])
def home():
    return "<h1>Distant Reading Archive</h1><p>This site is a prototype API for distant reading of science fiction novels.</p>"


@app.route("/next_app", methods=["GET"])
def next_app():
    sem.acquire()
    app_name = globe_wrapper.next_app()
    sem.release()
    return "Done: " + app_name


@app.route("/by_name", methods=["GET"])
def by_name():
    app_name = request.args.get("name", default=None, type=str)
    if app_name is None:
        return "Invalid App Name"

    sem.acquire()
    success = globe_wrapper.app_by_name(app_name)
    sem.release()

    if success:
        return "Done"
    else:
        return "Failed"


@app.route("/list_apps", methods=["GET"])
def list_apps():
    sem.acquire()
    all_apps = globe_wrapper.get_all_app_names()
    sem.release()

    return ", ".join(all_apps)


if __name__ == "__main__":
    app.run(host='0.0.0.0', threaded=False, processes=1)
