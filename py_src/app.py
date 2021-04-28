import flask
from flask import request, redirect, render_template

import threading

from globe_wrapper import GlobeWrapper

app = flask.Flask(__name__)

globe_wrapper = GlobeWrapper()
sem = threading.Semaphore()


@app.route("/", methods=["GET"])
def home():
    res = render_template("appselect.html", app_names=globe_wrapper.get_all_app_names())
    return res


@app.route("/next_app", methods=["GET"])
def next_app():
    sem.acquire()
    app_name = globe_wrapper.next_app()
    sem.release()
    return redirect("/", code=302)


@app.route("/by_name", methods=["GET"])
def by_name():
    app_name = request.args.get("name", default=None, type=str)
    if app_name is None:
        return "Invalid App Name"

    sem.acquire()
    success = globe_wrapper.app_by_name(app_name)
    sem.release()

    if success:
        return redirect("/", code=302)
    else:
        return "Failed"


if __name__ == "__main__":
    app.run(host="0.0.0.0", processes=1)
