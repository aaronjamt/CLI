from .base import CanvasObject

class Assignment(CanvasObject):
	def __init__(self, course_id, *args, **kwargs):
		self.course_id = course_id
		super().__init__(*args, *kwargs)

	def submit(self):
		response = self._canvas._request("courses/{self.course_id}/assignments/{self.id}/submissions", method="POST")
