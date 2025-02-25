class CanvasObject:
	""" Canvas object base class """
	def __init__(self, canvas_api, attrs):
		self._canvas = canvas_api
		self._attrs = attrs

	def __getattr__(self, attr):
		if attr in self._attrs:
			return self._attrs[attr]
		raise AttributeError(f"type object '{self.__class__.__name__}' has no attribute '{attr}'")
