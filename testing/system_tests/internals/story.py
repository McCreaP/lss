class Story:

    def __init__(self, raw_story):
        self.__raw_history = raw_story

    def get_raw(self, key):
        return self.__raw_history[key]

    def get_items(self):
        return self.__raw_history.items()