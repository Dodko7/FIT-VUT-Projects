---
title: "{{ env.FAILURE_TYPE }} Failure on {{ env.OS }} - {{ env.DATETIME }}"
labels: [bug, failure]
assignees: chadmee
---

## {{ env.FAILURE_TYPE }} Failure Report

{{ env.FAILURE_TYPE }} has failed on the **{{ env.OS }}** platform in the most recent commit.

### Details

- **Commit:** {{ env.GH_SHA }}
- **Branch:** {{ env.GH_REF_NAME }}
- **Triggered by:** {{ env.GH_ACTOR }}
- **Run URL:** [GitHub Actions Run](https://github.com/{{ env.GH_REPOSITORY }}/actions/runs/{{ env.GH_RUN_ID }})

### Commit Message

```text
{{ env.GH_HEAD_COMMIT }}
```

### Output

```text
{{ env.TEST_OUTPUT }}
```

### Summary

{{ env.TEST_SUMMARY }}
