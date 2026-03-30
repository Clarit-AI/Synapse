# Session Start Guide

Use this file to choose the right prompt for a new session.

## If you want the agent to detect the next roadmap phase automatically
Use:

- `docs/prompts/master-roadmap-orchestrator.md`

This is the right default when you want the session to inspect repo state, determine the next unfinished phase, and execute it.

## If you already know which roadmap phase you want
Use one of:

1. `docs/prompts/phase-2-policy-unification.md`
2. `docs/prompts/phase-3-rk3588-validation.md`
3. `docs/prompts/phase-4-artifact-cache-strategy.md`
4. `docs/prompts/phase-5-hybrid-quant-decision-gate.md`

## If the session is running on or against the ROCK 5 / RK3588 target
Use one of:

1. `docs/prompts/rock5/build-and-environment-validation.md`
2. `docs/prompts/rock5/hybrid-runtime-benchmark.md`
3. `docs/prompts/rock5/manifest-tuning.md`
4. `docs/prompts/rock5/strict-mode-and-fallback-audit.md`
5. `docs/prompts/rock5/phase-advance-decision.md`

## Recommended defaults
- Unknown next step: `docs/prompts/master-roadmap-orchestrator.md`
- Desktop implementation session: the relevant phase prompt under `docs/prompts/`
- On-device ROCK 5 validation session: the relevant prompt under `docs/prompts/rock5/`
