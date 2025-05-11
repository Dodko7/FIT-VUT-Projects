---
title: "{{ env.FAILURE_TYPE }} Failure on {{ env.OS }}"
labels: [bug, failure]
assignees: chadmee
---

## {{ env.FAILURE_TYPE }} Failure Report

{{ env.FAILURE_TYPE }} has failed on the **{{ env.OS }}** platform in the most recent commit.

### Details
- **Commit:** ${{ github.sha }}
- **Branch:** ${{ github.ref_name }}
- **Triggered by:** ${{ github.actor }}
- **Run URL:** https://github.com/${{ github.repository }}/actions/runs/${{ github.run_id }}

### Commit Message
```
${{ github.event.head_commit.message }}
```

### Test Output
```
{{ env.TEST_OUTPUT }}
```

### Summary
{{ env.TEST_SUMMARY }}
