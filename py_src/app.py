import flask
from flask import request, redirect, render_template

import threading

from globe_wrapper import GlobeWrapper

app = flask.Flask(__name__)

globe_wrapper = GlobeWrapper()
sem = threading.Semaphore()


@app.route("/", methods=["GET"])
def home():
    res = render_template(
        "app_select.html",
        apps=globe_wrapper.get_all_apps(),
        running_app_name=globe_wrapper.running_app_name
    )
    return res

@app.route("/add_horizontal_offset", methods=["GET", "POST"])
def add_horizontal_offset():
    offset = request.args.get("offset", default=None, type=int)

    globe_wrapper.set_horizontal_offset(globe_wrapper.get_horizontal_offset() + offset)
    return redirect("/", code=302)

@app.route("/launch_app", methods=["GET", "POST"])
def launch_app():

    app_name = request.args.get("name", default=None, type=str)

    apps = globe_wrapper.get_all_apps()
    if app_name is None or app_name not in apps:
        return "Invalid App Name"

    app = apps[app_name]

    args = []
    for arg in app["args"]:
        arg_name = arg["name"]
        arg_val = None
        if "options" in arg:
            arg_val = request.args.get(f"{arg_name}", default=None, type=str)
            if arg_val in arg["options"]:
                arg_val = arg["options"][arg_val]
            else:
                return "Failed: Invalid option for arg {}".format(arg_name)
        elif "type" in arg and arg["type"] == "str":
            arg_val = request.args.get(f"{arg_name}", default=None, type=str)
        elif "type" in arg and arg["type"] == "int":
            arg_val = request.args.get(f"{arg_name}", default=None, type=int)
        elif "type" in arg and arg["type"] == "float":
            arg_val = request.args.get(f"{arg_name}", default=None, type=float)
            
        args.append(arg_val)

    sem.acquire()
    success = globe_wrapper.app_by_name(app_name, args)
    sem.release()

    if success:
        return redirect("/", code=302)
    else:
        return "Failed"


@app.route("/configure_app", methods=["GET"])
def configure_app():
    app_name = request.args.get("name", default=None, type=str)

    apps = globe_wrapper.get_all_apps()

    if app_name is None or app_name not in apps:
        return "Invalid App Name"

    app = apps[app_name]

    simple_args = []
    options_args = []

    if len(app["args"]) == 0:
        return redirect("/launch_app?name=" + app_name, code=302)
    else:
        for arg in app["args"]:
            if arg["type"] == "options":
                options_args.append(arg)
            elif arg["type"] == "str":
                simple_args.append(arg)
            else:
                raise NotImplementedError()

        res = render_template(
            "configure_app.html",
            app_name=app_name,
            app=app,
            simple_args=simple_args,
            options_args=options_args,
        )
    return res


if __name__ == "__main__":
    app.run(host="0.0.0.0", processes=1)
