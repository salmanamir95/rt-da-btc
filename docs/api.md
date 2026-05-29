Yes, **all public market data APIs from Binance are 100% free**. You do not need to purchase a premium subscription, nor do you even need an API key to stream public WebSocket endpoints.

However, because you are building a high-performance system, you are gated by **Rate Limits and IP Weights** instead of money. If your C++/Rust application spams their servers incorrectly, Binance will automatically drop your connection or execute a temporary IP ban.

Here are the precise endpoints you need to configure for your platform, along with the system constraints you must program against.

---

## 1. The Spot Endpoints (Free)

### Live WebSockets (Streaming Ingestion)

* **Base URL:** `wss://stream.binance.com:9443/stream`
* **Streams to subscribe to:**
* **Prices & Wicks:** `btcusdt@kline_1s`
* **Order Book Support/Resistance:** `btcusdt@depth5@100ms`
* **Whale Accumulation (Taker Buy Vol):** `btcusdt@aggTrade`


* **System Rule:** You can pack up to 1,024 different streams into a single WebSocket connection.

---

## 2. The Futures Endpoints (Free)

### Live WebSockets (Streaming Ingestion)

* **Base URL:** `wss://fstream.binance.com/public/stream`
* **Streams to subscribe to:**
* **Prices & Wicks:** `btcusdt@kline_1s`
* **Order Book Support/Resistance:** `btcusdt@depth5@100ms`
* **Liquidations:** `btcusdt@forceOrder`



### REST API (Polled Background Worker)

Because certain metrics are macro-variables rather than real-time streams, your background worker needs to send asynchronous HTTP requests here:

* **Base URL:** `https://fapi.binance.com`
* **Endpoints:**
* **Open Interest:** `GET /fapi/v1/openInterest?symbol=BTCUSDT`
* **Long/Short Ratios:** `GET /fapi/v1/data/globalLongShortAccountRatio?symbol=BTCUSDT&period=5m`



---

## 3. Critical Systems Constraints (How to avoid getting banned)

When writing your C++/Rust ingestion loop, you must code defensively against Binance's rate-limiting firewall.

### A. The WebSocket Message Threshold

* **The Limit:** You are allowed a maximum of **5 incoming network messages per second** over a single WebSocket connection.
* **The Guardrail:** This limit applies to messages *you send* to the server (like `SUBSCRIBE` or `UNSUBSCRIBE` JSON commands, and your mandatory `PONG` frames). If you loop a subscription function incorrectly or spam connection parameters, your IP will be disconnected instantly.

### B. The REST API Weight System

* **The Limit:** 6,000 request weight units per minute per IP address.
* **The Guardrail:** Every single HTTP request you send has a designated "weight." Simple endpoints like checking server time cost 1 unit, but pulling heavy market metrics can cost 2 to 5+ units per request.
* **Systems Fix:** If your background worker polls the `openInterest` endpoint every 1 second, it's perfectly fine. But if you spawn an un-throttled loop hitting it as fast as possible without a timer delay, your application will receive an **HTTP 429 (Too Many Requests)** error. If you ignore this error, Binance will upgrade it to an **HTTP 418 IP Ban**, locking your development machine out of their servers for up to 3 days.

### C. The WebSocket Connection Lifespan

* **The Limit:** A single WebSocket connection to Binance is only valid for **24 hours**.
* **Systems Fix:** At the 24-hour mark, the Binance server will force-disconnect your C++/Rust application. Your code must feature an automated reconnection state machine. It needs to catch the disconnect event gracefully, flush the active memory buffers to ClickHouse/DuckDB, open a fresh socket descriptor, and re-subscribe to the data channels without crashing your system.
