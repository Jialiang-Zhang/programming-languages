#!/usr/bin/env python3
"""Recompute CCF CSP position-family statistics and rolling backtest.

Usage:
    python scripts/analyze.py
    python scripts/analyze.py --input data/questions.jsonl --half-life 10 --alpha 0.25
"""
from __future__ import annotations

import argparse
import json
import math
from collections import Counter
from pathlib import Path
from typing import Iterable


def load_rows(path: Path) -> list[dict]:
    rows: list[dict] = []
    files = sorted(path.glob("questions-q*.jsonl")) if path.is_dir() else [path]
    if not files:
        raise ValueError(f"{path}: no questions-q*.jsonl files")
    for file_path in files:
        with file_path.open("r", encoding="utf-8") as f:
            for line_no, line in enumerate(f, 1):
                line = line.strip()
                if not line:
                    continue
                try:
                    row = json.loads(line)
                except json.JSONDecodeError as exc:
                    raise ValueError(f"{file_path}:{line_no}: invalid JSON: {exc}") from exc
                required = {"session_no", "position", "primary_family"}
                missing = required - row.keys()
                if missing:
                    raise ValueError(
                        f"{file_path}:{line_no}: missing fields {sorted(missing)}"
                    )
                rows.append(row)
    if not rows:
        raise ValueError(f"{path}: no records")
    return rows


def fit_probabilities(
    rows: Iterable[dict],
    *,
    position: int,
    current_session: int,
    families: list[str],
    half_life: float,
    alpha: float,
) -> dict[str, float]:
    selected = [r for r in rows if int(r["position"]) == position]
    counts = Counter(r["primary_family"] for r in selected)
    weighted = Counter()

    for row in selected:
        session_no = int(row["session_no"])
        # The 2013-12 pilot is treated as older than official session 1.
        effective_session = session_no if session_no > 0 else -1
        age = max(0, current_session - effective_session)
        weight = 0.5 ** (age / half_life)
        weighted[row["primary_family"]] += weight

    all_total = sum(counts.values()) + alpha * len(families)
    recent_total = sum(weighted.values()) + alpha * len(families)
    probabilities: dict[str, float] = {}

    for family in families:
        p_all = (counts[family] + alpha) / all_total
        p_recent = (weighted[family] + alpha) / recent_total
        probabilities[family] = 0.35 * p_all + 0.65 * p_recent

    normalizer = sum(probabilities.values())
    return {k: v / normalizer for k, v in probabilities.items()}


def rolling_backtest(
    rows: list[dict],
    *,
    start_session: int,
    end_session: int,
    families: list[str],
    half_life: float,
    alpha: float,
) -> dict[int, dict[str, float]]:
    result: dict[int, dict[str, float]] = {}
    for position in range(1, 6):
        top1 = 0
        top2 = 0
        count = 0
        for target_session in range(start_session, end_session + 1):
            target = [
                r for r in rows
                if int(r["session_no"]) == target_session
                and int(r["position"]) == position
            ]
            if not target:
                continue
            train = [r for r in rows if int(r["session_no"]) < target_session]
            probs = fit_probabilities(
                train,
                position=position,
                current_session=target_session,
                families=families,
                half_life=half_life,
                alpha=alpha,
            )
            ranking = sorted(probs, key=probs.get, reverse=True)
            actual = target[0]["primary_family"]
            top1 += int(actual == ranking[0])
            top2 += int(actual in ranking[:2])
            count += 1

        result[position] = {
            "n": count,
            "top1": top1 / count if count else math.nan,
            "top2": top2 / count if count else math.nan,
        }
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--input",
        type=Path,
        default=Path(__file__).resolve().parents[1] / "data",
    )
    parser.add_argument("--half-life", type=float, default=10.0)
    parser.add_argument("--alpha", type=float, default=0.25)
    parser.add_argument("--backtest-start", type=int, default=23)
    parser.add_argument("--backtest-end", type=int, default=42)
    args = parser.parse_args()

    if args.half_life <= 0:
        raise SystemExit("--half-life must be positive")
    if args.alpha < 0:
        raise SystemExit("--alpha must be non-negative")

    rows = load_rows(args.input)
    families = sorted({r["primary_family"] for r in rows})
    current_session = max(int(r["session_no"]) for r in rows)

    print(f"records={len(rows)}, families={len(families)}, latest_session={current_session}")
    for position in range(1, 6):
        probs = fit_probabilities(
            rows,
            position=position,
            current_session=current_session,
            families=families,
            half_life=args.half_life,
            alpha=args.alpha,
        )
        top = sorted(probs.items(), key=lambda item: item[1], reverse=True)[:5]
        print(f"Q{position}: " + ", ".join(f"{name}={value:.1%}" for name, value in top))

    print("\nRolling backtest:")
    backtest = rolling_backtest(
        rows,
        start_session=args.backtest_start,
        end_session=args.backtest_end,
        families=families,
        half_life=args.half_life,
        alpha=args.alpha,
    )
    total_n = sum(v["n"] for v in backtest.values())
    weighted_top1 = sum(v["n"] * v["top1"] for v in backtest.values()) / total_n
    weighted_top2 = sum(v["n"] * v["top2"] for v in backtest.values()) / total_n
    for position, values in backtest.items():
        print(
            f"Q{position}: n={values['n']}, "
            f"top1={values['top1']:.0%}, top2={values['top2']:.0%}"
        )
    print(f"overall: n={total_n}, top1={weighted_top1:.0%}, top2={weighted_top2:.0%}")


if __name__ == "__main__":
    main()
