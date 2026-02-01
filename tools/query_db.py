#!/usr/bin/env python3
import sqlite3
import sys
from pathlib import Path

DATA_DIR = Path(__file__).resolve().parent.parent / "data"
DB_PATH = DATA_DIR / "slips.db"


def main() -> int:
    if not DB_PATH.exists():
        print(f"DB not found: {DB_PATH}")
        return 1

    query = sys.argv[1] if len(sys.argv) > 1 else ""
    if not query:
        print("Usage: query_db.py <name-substring>")
        return 2

    with sqlite3.connect(DB_PATH) as conn:
        rows = conn.execute(
            "SELECT name, phone, address, price, discount, total, timestamp FROM slips WHERE LOWER(name) LIKE ?",
            (f"%{query.lower()}%",),
        ).fetchall()

    if not rows:
        print("No matches.")
        return 0

    for row in rows:
        print(" | ".join(str(value) for value in row))

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
