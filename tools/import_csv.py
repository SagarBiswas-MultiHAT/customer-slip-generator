#!/usr/bin/env python3
import csv
import sqlite3
from pathlib import Path

DATA_DIR = Path(__file__).resolve().parent.parent / "data"
CSV_PATH = DATA_DIR / "slips.csv"
DB_PATH = DATA_DIR / "slips.db"


def main() -> int:
    if not CSV_PATH.exists():
        print(f"CSV not found: {CSV_PATH}")
        return 1

    DATA_DIR.mkdir(parents=True, exist_ok=True)

    with sqlite3.connect(DB_PATH) as conn:
        conn.execute(
            """
            CREATE TABLE IF NOT EXISTS slips (
                name TEXT,
                phone TEXT,
                address TEXT,
                price REAL,
                discount REAL,
                total REAL,
                timestamp TEXT
            )
            """
        )
        conn.execute("DELETE FROM slips")

        with CSV_PATH.open(newline="", encoding="utf-8") as handle:
            reader = csv.DictReader(handle)
            rows = [
                (
                    row.get("name", ""),
                    row.get("phone", ""),
                    row.get("address", ""),
                    float(row.get("price", 0.0)),
                    float(row.get("discount", 0.0)),
                    float(row.get("total", 0.0)),
                    row.get("timestamp", ""),
                )
                for row in reader
            ]

        conn.executemany(
            "INSERT INTO slips (name, phone, address, price, discount, total, timestamp) VALUES (?, ?, ?, ?, ?, ?, ?)",
            rows,
        )
        conn.commit()

    print(f"Imported {len(rows)} rows into {DB_PATH}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
