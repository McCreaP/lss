class InvalidJobException(Exception):
    
    def __init__(self, job_id, message):
        self.__job_id = job_id
        self.__message = message
        
    def __str__(self):
        return "Invalid job %s: " + self.__message