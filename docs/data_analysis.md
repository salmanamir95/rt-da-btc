# RT-DA: Data Analysis Metrics & Results

The `RT-DA` Analytics Engine processes high-frequency cryptocurrency market data across a rolling, lock-free memory window (typically 8 Klines/ticks). For each window, the engine simultaneously calculates four distinct statistical metrics across five dimensions (Open, Close, High, Low, and Volume) using 20 concurrent threads.

This document breaks down the specific results provided by the `PipelineKline` execution.

---

## 1. The Five Dimensions
Every statistical factory computes its specific metric independently for the following five fields:

- **OP (Open Price):** The price at the beginning of the time interval.
- **CP (Close Price):** The price at the end of the time interval.
- **HP (High Price):** The highest price reached during the time interval.
- **LP (Low Price):** The lowest price reached during the time interval.
- **Vol (Volume):** The total asset volume traded during the interval.

---

## 2. Statistical Factories & Metrics Provided

### A. Mean (Average) `MeanResult`
The arithmetic average of the values within the current rolling window.

- **Purpose:** Smooths out short-term price fluctuations and highlights the underlying trend.
- **Implementation Note:** Employs a zero-branch unrolled speed lane for full windows (8/8 elements) to bypass loop counters and branch mispredictions, calculating the sum instantly.

### B. Mode (Most Frequent) `ModeResult`
The value that appears most often in the rolling window, representing price clusters and support/resistance walls.

- **Purpose:** Identifies where the market is spending the most time. High frequency at a specific price point often indicates strong psychological support or resistance.
- **Implementation Note:** Because floating-point tick data is noisy, the engine bins values to the nearest integer unit (e.g., nearest $1 for BTC) to find clusters. If every price in the window is completely unique (max frequency = 1), the engine gracefully falls back to the arithmetic mean to prevent data skewing on visualization dashboards.

### C. Median (Middle Value) `MedianResult`
The middle value when the data points in the window are sorted in ascending order.

- **Purpose:** Provides a robust measure of central tendency that is resistant to sudden, extreme outliers (e.g., flash crashes or abnormal liquidation wicks). 
- **Implementation Note:** Uses `std::sort` internally on a stack-allocated cache array to quickly determine the midpoint without triggering heap allocations.

### D. Standard Deviation (Volatility) `StdDevResult`
Measures the amount of variation or dispersion of the dataset relative to its mean.

- **Purpose:** Acts as a real-time volatility index. A high standard deviation indicates a chaotic, wide-ranging market, whereas a low standard deviation indicates consolidation. This is critical for computing Bollinger Bands or adjusting risk dynamically.
- **Implementation Note:** Computes the variance (average of the squared differences from the Mean) and returns the square root. Calculated concurrently alongside the other metrics.

---

## 3. Data Output Example
When the pipeline successfully computes a window, it generates a synchronized packet of structs. A conceptual representation of a single pipeline flush looks like this:

```json
{
  "timestamp": 1717154382000,
  "Mean": { "OP": 68100.5, "CP": 68105.2, "HP": 68150.0, "LP": 68080.0, "Vol": 12.5 },
  "Mode": { "OP": 68100.0, "CP": 68106.0, "HP": 68150.0, "LP": 68080.0, "Vol": 12.0 },
  "Median": { "OP": 68101.0, "CP": 68104.5, "HP": 68145.0, "LP": 68085.0, "Vol": 11.8 },
  "StdDev": { "OP": 15.2, "CP": 18.5, "HP": 25.0, "LP": 10.5, "Vol": 3.2 }
}
```

This output payload is then ready to be bulk-inserted into a time-series database (like ClickHouse or DuckDB) where Python BI tools can visualize the relationships between standard deviations and price clusters in real-time.
