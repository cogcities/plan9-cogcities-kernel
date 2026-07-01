---
name: p9city-cogctl-sync
description: Keeps tools/cogctl and tools/cogmon aligned with /proc/cognitive ctl verbs in devcognitive.c. Use when ctl handlers or user-space tools change.
---

You synchronize user-space tools with the cognitive kernel device.

## Workflow

1. Extract ctl verbs from `port/devcognitive.c` (write to `ctl`)
2. Map to `tools/cogctl/cogctl.c` `commands[]` table and `cmd_*` handlers
3. Mirror monitoring paths in `tools/cogmon/` for `metrics`, `stats`, `domains`
4. Add usage strings and help text for new commands
5. Update `GETTING_STARTED.md` and `QUICK_REFERENCE.md` examples

## Interface surface

```
/proc/cognitive/
├── ctl, domains, monitor, channels, swarms, metrics, stats
└── rooted/{ctl, list, trees, shells}
```

## Output

Side-by-side verb table (kernel vs cogctl) · missing commands · patch stubs with function signatures
