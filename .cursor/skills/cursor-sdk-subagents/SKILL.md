---
name: cursor-sdk-subagents
description: Creates Cursor SDK subagents for cogcities/plan9-cogcities-kernel (Plan 9 cognitive kernel, /proc/cognitive, cogctl/cogmon). Use when automating kernel wiring, cogctl sync, matula/regression tests, or demo-doc consistency for this repo.
---

# Cursor SDK Subagents — plan9-cogcities-kernel

Repo: **cogcities/plan9-cogcities-kernel** · Plan 9 kernel fork · `port/cognitive.c` · `/proc/cognitive` · `mk` build · no in-repo CI.

Compose **subagent authoring** with **SDK domain knowledge** — read `/sdk` for API truth; use this skill for repo-specific subagent design.

## Repo context

| Area | Paths |
|------|-------|
| Cognitive kernel | `port/cognitive.c`, `port/devcognitive.c` |
| Kernel config | `pc/pc` (add `cognitive` under `dev` to expose device) |
| User tools | `tools/cogctl/`, `tools/cogmon/` |
| Demos | `tools/demos/*.c` — traffic, energy, esn, matula, etc. |
| Tests | `tools/tests/run-tests` (requires live `/proc/cognitive`) |
| Matula unit tests | `tools/demos/matula-test.c` |
| Existing agents | `.github/agents/p9city.md` and 80+ Copilot prompts |
| Docs | `docs/`, `GETTING_STARTED.md`, `IMPLEMENTATION_SUMMARY.md` |

**Build:** `mk` from root or `cd pc && mk`. **Gap:** `cognitive` device not yet in default `pc/pc` config.

## SDK runtime choice for this repo

| Task | Runtime | Why |
|------|---------|-----|
| Kernel wiring PRs, cogctl sync, doc consistency | **Cloud** `repos: [{ name: "cogcities/plan9-cogcities-kernel" }]` | Plan 9 toolchain not on cloud VM by default — agent reviews/edits source |
| Live `run-tests` against `/proc/cognitive` | **Local** on Plan 9 VM | Requires running kernel with device loaded |

Cloud agents edit source and draft PRs; runtime validation stays on Plan 9 hosts.

## Installed subagents

| Subagent | Use when |
|----------|----------|
| [p9city-kernel-wirer](../../agents/p9city-kernel-wirer.md) | Wiring `cognitive` into arch configs (`pc/pc`, etc.) |
| [p9city-cogctl-sync](../../agents/p9city-cogctl-sync.md) | Keeping `cogctl` commands aligned with `devcognitive.c` ctl verbs |
| [p9city-test-runner](../../agents/p9city-test-runner.md) | `matula-test`, `run-tests` preflight and golden diffs |

## Templates

- [sdk-integration-builder.md](templates/sdk-integration-builder.md) — cloud agent for kernel-integration PRs
- [sdk-ci-automation.md](templates/sdk-ci-automation.md) — scaffold CI when added (today: no workflows)
- [sdk-run-debugger.md](templates/sdk-run-debugger.md) — debug SDK jobs targeting this repo
- [p9city-kernel-wirer.md](templates/p9city-kernel-wirer.md) — subagent source template

## Example cloud prompt (kernel wiring PR)

```python
from cursor_sdk import Agent, AgentOptions, CloudAgentOptions

Agent.prompt(
    "Add cognitive device to pc/pc kernel config, verify devcognitive.c registration, "
    "and draft a PR description with deployment steps from docs/implementation/deployment-guide.md.",
    AgentOptions(
        api_key=os.environ["CURSOR_API_KEY"],
        model="composer-2.5",
        cloud=CloudAgentOptions(
            repos=[{"name": "cogcities/plan9-cogcities-kernel", "branch": "master"}],
            auto_create_pr=True,
        ),
        skip_reviewer_request=True,
    ),
)
```

## Creation workflow

```
- [ ] Pick subagent purpose (kernel / proc / tools / demos / docs)
- [ ] Place in .cursor/agents/<name>.md
- [ ] Cross-check .github/agents/ for overlap — extend, don't duplicate
- [ ] Test: "Use the <name> subagent to …"
```

## SDK traps

Runtime explicit · disposal · `wait()` · `CursorAgentError` vs `status === "error"` · MCP re-pass on resume.

Docs: [TypeScript SDK](https://cursor.com/docs/sdk/typescript) · [Python SDK](https://cursor.com/docs/sdk/python)
