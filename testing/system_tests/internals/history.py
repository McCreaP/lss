from internals import lstory


jobs = {}


def record_job(j):
    jobs[j['id']] = j


def write(path, back_story):

    def for_one(k, v):
        if k == 'jobs':
            return list(jobs.values())
        elif k == 'context_changes':
            return list(v.items())
        else:
            return v

    story = {k: for_one(k, v) for k, v in back_story.items()}
    lstory.write(path, story)

