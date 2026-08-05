# OpenSpec for gen3recomp

This repository is specified before it is implemented.

```
openspec/
├── README.md                 ← you are here
├── config.yaml               ← project context injected into every change
├── product/                  ← durable product + architecture docs
│   ├── vision.md
│   ├── roadmap.md
│   ├── architecture.md
│   ├── milestones.md
│   ├── epics.md
│   ├── features.md
│   ├── decisions.md
│   ├── risks.md
│   ├── dependencies.md
│   └── future.md
├── specs/                    ← living behavior contracts (truth after archive)
│   ├── legal/
│   └── engineering-principles/
└── changes/                  ← one folder per incremental milestone
    ├── m01-project-skeleton/
    ├── m02-build-and-test-harness/
    ├── …
    └── archive/
```

## How to read this

1. [product/vision.md](product/vision.md) — why the project exists
2. [product/architecture.md](product/architecture.md) — module map and dependency rules
3. [product/decisions.md](product/decisions.md) — choices and rejected alternatives
4. [product/roadmap.md](product/roadmap.md) and [product/milestones.md](product/milestones.md) — order of delivery
5. [product/epics.md](product/epics.md) and [product/features.md](product/features.md) — scope breakdown
6. Active changes under `changes/m0X-…/` — proposal, delta specs, design, tasks, acceptance scenarios

Main specs start with constraints only. Feature behavior is added by archiving milestone changes, in order.

## Workflow

Default schema: `spec-driven` (`proposal → specs → design → tasks → apply → archive`).

In Cursor:

- `/opsx-explore` — think before changing the plan
- `/opsx-apply` — implement the current milestone only
- `/opsx-archive` — merge delta specs into `openspec/specs/`

Do not start `m0N+1` until `m0N` leaves a runnable, testable artifact.

## Rules of engagement

- Specs describe observable behavior. Designs describe approach. Tasks are checkboxes.
- No code until a milestone change is applied.
- Do not expand MVP scope into items listed in [product/future.md](product/future.md).
