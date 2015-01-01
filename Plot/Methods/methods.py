# background
from background import Background
# plot
from fill_poly import Fill_Poly
from line import Line
# layer_plot
from opaque import Opaque

def get_plot_class(level, conf):
    method = conf.get("method")
    if method == "line":
        return Line(level, conf)
    elif method == "fill_poly":
        return Fill_Poly(level, conf)
    raise BaseException, "Plotting method not found."

def get_layer_plot_class(level, layer_config):
    method = layer_config["method"]
    if method == "background":
        return Background(level, layer_config)
    elif method == "opaque":
        return Opaque(level, layer_config)

    raise BaseException, "Layer plotting method not found."