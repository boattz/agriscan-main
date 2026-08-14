#pragma once

const char* dashboard_html = R"rawliteral(<!DOCTYPE html>
<html lang="th">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title>Agriscan — Dashboard</title>
  <meta name="description" content="แดชบอร์ดแสดงผลข้อมูลเซ็นเซอร์ดิน Agriscan แบบ Real-time" />
  <link rel="preconnect" href="https://fonts.googleapis.com" />
  <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin />
  <link href="https://fonts.googleapis.com/css2?family=Prompt:wght@300;400;500;600;700&family=JetBrains+Mono:wght@400;600&display=swap" rel="stylesheet" />

  <style>
    /* ══════════════════════════════════════════════
       Design Tokens
    ══════════════════════════════════════════════ */
    :root {
      --green-50:  #f0fdf4;
      --green-100: #dcfce7;
      --green-200: #bbf7d0;
      --green-300: #86efac;
      --green-400: #4ade80;
      --green-500: #22c55e;
      --green-600: #16a34a;
      --green-700: #15803d;
      --green-800: #166534;
      --green-900: #14532d;
      --green-950: #052e16;

      --teal-400:  #2dd4bf;
      --teal-500:  #14b8a6;
      --teal-600:  #0d9488;

      --amber-400: #fbbf24;
      --amber-500: #f59e0b;

      --red-400:   #f87171;
      --red-500:   #ef4444;

      --blue-400:  #60a5fa;
      --blue-500:  #3b82f6;

      --bg-base:      #0a1a12;
      --bg-card:      #0f2418;
      --bg-card-2:    #122b1e;
      --bg-glass:     rgba(22, 101, 52, 0.18);
      --border-dim:   rgba(34, 197, 94, 0.15);
      --border-glow:  rgba(34, 197, 94, 0.45);

      --text-primary:  #e2fce9;
      --text-secondary:#93c9a5;
      --text-muted:    #4e7860;

      --shadow-glow:   0 0 32px rgba(34, 197, 94, 0.12);
      --shadow-card:   0 4px 24px rgba(0,0,0,0.5);

      --radius-card:  18px;
      --radius-pill:  999px;

      --font-body: 'Prompt', sans-serif;
      --font-mono: 'JetBrains Mono', monospace;
    }

    /* ══════════════════════════════════════════════
       Reset & Base
    ══════════════════════════════════════════════ */
    *, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }

    html { scroll-behavior: smooth; }

    body {
      font-family: var(--font-body);
      background: var(--bg-base);
      color: var(--text-primary);
      min-height: 100vh;
      overflow-x: hidden;
    }

    /* Animated background */
    body::before {
      content: '';
      position: fixed;
      inset: 0;
      background:
        radial-gradient(ellipse 80% 60% at 20% 10%, rgba(22,163,74,0.12) 0%, transparent 60%),
        radial-gradient(ellipse 60% 80% at 80% 90%, rgba(13,148,136,0.08) 0%, transparent 60%),
        radial-gradient(ellipse 40% 40% at 60% 40%, rgba(74,222,128,0.04) 0%, transparent 50%);
      pointer-events: none;
      z-index: 0;
    }

    /* Floating particles */
    body::after {
      content: '';
      position: fixed;
      inset: 0;
      background-image:
        radial-gradient(1px 1px at 10% 20%, rgba(74,222,128,0.3) 0%, transparent 100%),
        radial-gradient(1px 1px at 80% 15%, rgba(74,222,128,0.2) 0%, transparent 100%),
        radial-gradient(1px 1px at 50% 60%, rgba(45,212,191,0.3) 0%, transparent 100%),
        radial-gradient(1px 1px at 90% 70%, rgba(74,222,128,0.15) 0%, transparent 100%),
        radial-gradient(1px 1px at 30% 80%, rgba(45,212,191,0.2) 0%, transparent 100%);
      pointer-events: none;
      z-index: 0;
    }

    /* ══════════════════════════════════════════════
       Layout
    ══════════════════════════════════════════════ */
    .page-wrapper {
      position: relative;
      z-index: 1;
      max-width: 1280px;
      margin: 0 auto;
      padding: 0 16px 48px;
    }

    /* ══════════════════════════════════════════════
       Header
    ══════════════════════════════════════════════ */
    .header {
      display: flex;
      align-items: center;
      justify-content: space-between;
      flex-wrap: wrap;
      gap: 16px;
      padding: 28px 0 24px;
      border-bottom: 1px solid var(--border-dim);
      margin-bottom: 32px;
    }

    .header-left {
      display: flex;
      align-items: center;
      gap: 16px;
    }

    .logo-wrap {
      width: 52px; height: 52px;
      border-radius: 14px;
      background: linear-gradient(135deg, var(--green-600), var(--teal-600));
      display: grid;
      place-items: center;
      box-shadow: 0 0 24px rgba(22,197,94,0.35);
      flex-shrink: 0;
    }

    .logo-wrap svg { width: 28px; height: 28px; fill: white; }

    .header-title h1 {
      font-size: clamp(1.15rem, 3vw, 1.5rem);
      font-weight: 700;
      color: var(--text-primary);
      letter-spacing: -0.3px;
    }

    .header-title p {
      font-size: 0.78rem;
      color: var(--text-muted);
      margin-top: 2px;
    }

    .header-right {
      display: flex;
      align-items: center;
      gap: 14px;
      flex-wrap: wrap;
    }

    /* Status badge */
    .status-badge {
      display: flex;
      align-items: center;
      gap: 8px;
      padding: 7px 16px;
      border-radius: var(--radius-pill);
      border: 1px solid var(--border-dim);
      background: var(--bg-card);
      font-size: 0.78rem;
      font-weight: 500;
      transition: all 0.4s ease;
    }

    .status-badge.online {
      border-color: rgba(34,197,94,0.4);
      background: rgba(22,101,52,0.35);
      color: var(--green-300);
    }

    .status-badge.offline {
      border-color: rgba(239,68,68,0.3);
      background: rgba(239,68,68,0.08);
      color: var(--red-400);
    }

    .status-badge.connecting {
      border-color: rgba(251,191,36,0.3);
      background: rgba(251,191,36,0.06);
      color: var(--amber-400);
    }

    .status-dot {
      width: 8px; height: 8px;
      border-radius: 50%;
      background: currentColor;
      flex-shrink: 0;
    }

    .status-badge.online .status-dot  { animation: pulse-dot 2s infinite; }
    .status-badge.connecting .status-dot { animation: blink-dot 0.8s infinite; }

    @keyframes pulse-dot {
      0%, 100% { opacity: 1; box-shadow: 0 0 0 0 rgba(34,197,94,0.5); }
      50% { opacity: 0.8; box-shadow: 0 0 0 5px rgba(34,197,94,0); }
    }
    @keyframes blink-dot {
      0%, 100% { opacity: 1; }
      50% { opacity: 0.2; }
    }

    /* Last update */
    .last-update {
      font-size: 0.75rem;
      color: var(--text-muted);
      font-family: var(--font-mono);
    }

    /* IP badge */
    .ip-badge {
      font-size: 0.72rem;
      font-family: var(--font-mono);
      color: var(--teal-400);
      background: rgba(13,148,136,0.12);
      border: 1px solid rgba(13,148,136,0.25);
      padding: 4px 10px;
      border-radius: var(--radius-pill);
    }

    /* ══════════════════════════════════════════════
       Connecting overlay
    ══════════════════════════════════════════════ */
    .connecting-banner {
      display: none;
      align-items: center;
      gap: 14px;
      padding: 16px 22px;
      border-radius: var(--radius-card);
      background: rgba(251,191,36,0.07);
      border: 1px solid rgba(251,191,36,0.25);
      margin-bottom: 24px;
      animation: slide-down 0.3s ease;
    }
    .connecting-banner.visible { display: flex; }

    @keyframes slide-down {
      from { opacity: 0; transform: translateY(-8px); }
      to   { opacity: 1; transform: translateY(0); }
    }

    .connecting-spinner {
      width: 20px; height: 20px;
      border: 2px solid rgba(251,191,36,0.2);
      border-top-color: var(--amber-400);
      border-radius: 50%;
      animation: spin 0.8s linear infinite;
      flex-shrink: 0;
    }
    @keyframes spin { to { transform: rotate(360deg); } }

    .connecting-banner p { font-size: 0.88rem; color: var(--amber-400); }
    .connecting-banner small { font-size: 0.75rem; color: var(--text-muted); }

    /* ══════════════════════════════════════════════
       Cards Grid
    ══════════════════════════════════════════════ */
    .cards-grid {
      display: grid;
      grid-template-columns: repeat(auto-fill, minmax(260px, 1fr));
      gap: 18px;
      margin-bottom: 24px;
    }

    /* ── Base card ── */
    .card {
      background: var(--bg-card);
      border: 1px solid var(--border-dim);
      border-radius: var(--radius-card);
      padding: 22px 22px 20px;
      box-shadow: var(--shadow-card);
      transition: transform 0.25s ease, box-shadow 0.25s ease, border-color 0.25s ease;
      position: relative;
      overflow: hidden;
    }

    .card::before {
      content: '';
      position: absolute;
      top: 0; left: 0; right: 0;
      height: 2px;
      background: var(--card-accent, linear-gradient(90deg, var(--green-500), var(--teal-500)));
      opacity: 0.7;
      transition: opacity 0.25s;
    }

    .card:hover {
      transform: translateY(-4px);
      box-shadow: var(--shadow-card), 0 0 28px rgba(34,197,94,0.1);
      border-color: var(--border-glow);
    }
    .card:hover::before { opacity: 1; }

    /* ── Card header ── */
    .card-header {
      display: flex;
      align-items: center;
      justify-content: space-between;
      margin-bottom: 16px;
    }

    .card-label {
      font-size: 0.72rem;
      font-weight: 600;
      letter-spacing: 0.08em;
      text-transform: uppercase;
      color: var(--text-muted);
    }

    .card-icon {
      width: 36px; height: 36px;
      border-radius: 10px;
      display: grid;
      place-items: center;
      background: var(--icon-bg, rgba(34,197,94,0.12));
      flex-shrink: 0;
    }
    .card-icon svg { width: 18px; height: 18px; }

    /* ── Card value ── */
    .card-value {
      font-size: clamp(2rem, 5vw, 2.6rem);
      font-weight: 700;
      line-height: 1;
      font-family: var(--font-mono);
      color: var(--text-primary);
      transition: color 0.4s ease;
    }

    .card-unit {
      font-size: 0.8rem;
      color: var(--text-muted);
      font-family: var(--font-body);
      font-weight: 400;
      margin-left: 4px;
    }

    /* ── Status chip ── */
    .card-status {
      display: inline-flex;
      align-items: center;
      gap: 5px;
      font-size: 0.7rem;
      font-weight: 600;
      padding: 4px 10px;
      border-radius: var(--radius-pill);
      margin-top: 10px;
      transition: all 0.4s ease;
    }
    .card-status.ok     { background: rgba(34,197,94,0.12); color: var(--green-400); }
    .card-status.warn   { background: rgba(251,191,36,0.12); color: var(--amber-400); }
    .card-status.alert  { background: rgba(239,68,68,0.12); color: var(--red-400); }

    /* ── Progress bar (moisture) ── */
    .moisture-bar-wrap {
      margin-top: 14px;
    }

    .moisture-bar-track {
      height: 8px;
      background: rgba(255,255,255,0.06);
      border-radius: var(--radius-pill);
      overflow: hidden;
    }

    .moisture-bar-fill {
      height: 100%;
      border-radius: var(--radius-pill);
      background: var(--bar-color, var(--green-500));
      transition: width 0.8s cubic-bezier(0.4,0,0.2,1), background 0.5s ease;
      box-shadow: 0 0 8px var(--bar-color, var(--green-500));
    }

    .moisture-labels {
      display: flex;
      justify-content: space-between;
      margin-top: 6px;
    }
    .moisture-labels span {
      font-size: 0.65rem;
      color: var(--text-muted);
    }

    /* ── NPK card ── */
    .npk-card { --card-accent: linear-gradient(90deg, #a855f7, #3b82f6); }
    .npk-card .card-icon { --icon-bg: rgba(168,85,247,0.15); }

    .npk-bars { margin-top: 16px; display: flex; flex-direction: column; gap: 12px; }

    .npk-row {
      display: flex;
      align-items: center;
      gap: 10px;
    }

    .npk-label {
      font-size: 0.72rem;
      font-weight: 700;
      width: 16px;
      flex-shrink: 0;
      font-family: var(--font-mono);
    }
    .npk-label.n { color: #86efac; }
    .npk-label.p { color: #93c5fd; }
    .npk-label.k { color: #fdba74; }

    .npk-track {
      flex: 1;
      height: 6px;
      background: rgba(255,255,255,0.06);
      border-radius: var(--radius-pill);
      overflow: hidden;
    }

    .npk-fill {
      height: 100%;
      border-radius: var(--radius-pill);
      transition: width 0.8s cubic-bezier(0.4,0,0.2,1);
    }
    .npk-fill.n { background: linear-gradient(90deg, #22c55e, #86efac); }
    .npk-fill.p { background: linear-gradient(90deg, #3b82f6, #93c5fd); }
    .npk-fill.k { background: linear-gradient(90deg, #f97316, #fdba74); }

    .npk-val {
      font-size: 0.7rem;
      font-family: var(--font-mono);
      color: var(--text-secondary);
      width: 48px;
      text-align: right;
      flex-shrink: 0;
    }

    /* ══════════════════════════════════════════════
       Recommendations
    ══════════════════════════════════════════════ */
    .section-title {
      font-size: 0.8rem;
      font-weight: 600;
      letter-spacing: 0.08em;
      text-transform: uppercase;
      color: var(--text-muted);
      margin-bottom: 14px;
      display: flex;
      align-items: center;
      gap: 8px;
    }
    .section-title::after {
      content: '';
      flex: 1;
      height: 1px;
      background: var(--border-dim);
    }

    .rec-grid {
      display: grid;
      grid-template-columns: repeat(auto-fill, minmax(280px, 1fr));
      gap: 12px;
    }

    .rec-item {
      display: flex;
      align-items: flex-start;
      gap: 12px;
      padding: 14px 16px;
      border-radius: 12px;
      border: 1px solid transparent;
      animation: fade-in 0.4s ease;
      transition: transform 0.2s ease;
    }
    .rec-item:hover { transform: translateX(3px); }

    @keyframes fade-in {
      from { opacity: 0; transform: translateY(6px); }
      to   { opacity: 1; transform: translateY(0); }
    }

    .rec-item.ok      { background: rgba(34,197,94,0.06);  border-color: rgba(34,197,94,0.15);  }
    .rec-item.warn    { background: rgba(251,191,36,0.06); border-color: rgba(251,191,36,0.2);  }
    .rec-item.alert   { background: rgba(239,68,68,0.06);  border-color: rgba(239,68,68,0.18);  }
    .rec-item.info    { background: rgba(59,130,246,0.06); border-color: rgba(59,130,246,0.18); }

    .rec-icon {
      font-size: 1.2rem;
      line-height: 1;
      flex-shrink: 0;
      margin-top: 1px;
    }

    .rec-content { flex: 1; }

    .rec-title {
      font-size: 0.82rem;
      font-weight: 600;
      color: var(--text-primary);
      margin-bottom: 2px;
    }

    .rec-desc {
      font-size: 0.75rem;
      color: var(--text-secondary);
      line-height: 1.5;
    }

    /* ══════════════════════════════════════════════
       Footer
    ══════════════════════════════════════════════ */
    .footer {
      margin-top: 40px;
      padding-top: 20px;
      border-top: 1px solid var(--border-dim);
      display: flex;
      align-items: center;
      justify-content: space-between;
      flex-wrap: wrap;
      gap: 10px;
    }

    .footer p {
      font-size: 0.72rem;
      color: var(--text-muted);
    }

    .footer-controls { display: flex; gap: 10px; }

    .btn-refresh {
      display: flex;
      align-items: center;
      gap: 6px;
      padding: 7px 14px;
      border-radius: var(--radius-pill);
      border: 1px solid var(--border-dim);
      background: var(--bg-card);
      color: var(--text-secondary);
      font-family: var(--font-body);
      font-size: 0.78rem;
      cursor: pointer;
      transition: all 0.2s ease;
    }
    .btn-refresh:hover {
      border-color: var(--border-glow);
      color: var(--green-300);
      background: var(--bg-card-2);
    }
    .btn-refresh svg { width: 14px; height: 14px; }
    .btn-refresh.spinning svg { animation: spin 0.6s linear infinite; }

    /* ══════════════════════════════════════════════
       Skeleton loading
    ══════════════════════════════════════════════ */
    .skeleton {
      background: linear-gradient(90deg, rgba(255,255,255,0.04) 25%, rgba(255,255,255,0.08) 50%, rgba(255,255,255,0.04) 75%);
      background-size: 200% 100%;
      animation: shimmer 1.5s infinite;
      border-radius: 6px;
      color: transparent !important;
      user-select: none;
    }
    @keyframes shimmer {
      0%   { background-position: 200% 0; }
      100% { background-position: -200% 0; }
    }

    /* ══════════════════════════════════════════════
       Responsive
    ══════════════════════════════════════════════ */
    @media (max-width: 600px) {
      .header { flex-direction: column; align-items: flex-start; }
      .header-right { width: 100%; justify-content: space-between; }
      .cards-grid { grid-template-columns: 1fr; }
      .rec-grid { grid-template-columns: 1fr; }
    }

    /* ══════════════════════════════════════════════
       Value update flash
    ══════════════════════════════════════════════ */
    @keyframes value-flash {
      0%   { color: var(--green-300); text-shadow: 0 0 12px rgba(74,222,128,0.5); }
      100% { color: var(--text-primary); text-shadow: none; }
    }
    .flash { animation: value-flash 0.6s ease-out; }

    /* ══════════════════════════════════════════════
       Mock data banner
    ══════════════════════════════════════════════ */
    .mock-banner {
      display: none;
      padding: 10px 18px;
      border-radius: 10px;
      background: rgba(59,130,246,0.08);
      border: 1px solid rgba(59,130,246,0.2);
      font-size: 0.78rem;
      color: var(--blue-400);
      margin-bottom: 20px;
      align-items: center;
      gap: 8px;
    }
    .mock-banner.visible { display: flex; }
  </style>
</head>
<body>
<div class="page-wrapper">

  <!-- ═══ HEADER ═══ -->
  <header class="header">
    <div class="header-left">
      <div class="logo-wrap" aria-hidden="true">
        <!-- Plant icon -->
        <svg viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg">
          <path d="M12 2C9.24 2 7 4.24 7 7c0 2.09 1.24 3.88 3 4.73V20h2v-8.27A4.996 4.996 0 0 0 17 7c0-2.76-2.24-5-5-5zm-1 14H9v2h2v-2zm2 0h-1v2h1v-2z" opacity=".3"/>
          <path d="M12 2C9.24 2 7 4.24 7 7c0 2.09 1.24 3.88 3 4.73V22h4V11.73A4.996 4.996 0 0 0 17 7c0-2.76-2.24-5-5-5zm0 2c1.65 0 3 1.35 3 3s-1.35 3-3 3-3-1.35-3-3 1.35-3 3-3zm1 15h-2v-2h2v2zm0-4h-2v-5.08c.33.05.66.08 1 .08s.67-.03 1-.08V15z"/>
          <circle cx="5" cy="4" r="1.5" opacity=".5"/>
          <circle cx="19" cy="4" r="1.5" opacity=".5"/>
          <circle cx="3" cy="9" r="1" opacity=".4"/>
          <circle cx="21" cy="9" r="1" opacity=".4"/>
        </svg>
      </div>
      <div class="header-title">
        <h1>Agriscan</h1>
        <p>Real-time Soil Sensor Dashboard</p>
      </div>
    </div>

    <div class="header-right">
      <span class="ip-badge" id="ip-badge" onclick="changeIp()" title="คลิกเพื่อเปลี่ยน IP/Host">agriscan.local ⚙</span>
      <div class="status-badge connecting" id="status-badge">
        <span class="status-dot"></span>
        <span id="status-text">กำลังเชื่อมต่อ...</span>
      </div>
      <span class="last-update" id="last-update">--:--:--</span>
    </div>
  </header>

  <!-- ═══ CONNECTING BANNER ═══ -->
  <div class="connecting-banner" id="connecting-banner">
    <div class="connecting-spinner"></div>
    <div style="flex: 1; display: flex; align-items: center; justify-content: space-between; flex-wrap: wrap; gap: 8px;">
      <div>
        <p>กำลังเชื่อมต่อกับ ESP32...</p>
        <small id="retry-count">กำลังลอง retry...</small>
      </div>
      <button onclick="changeIp()" style="background: rgba(251,191,36,0.15); border: 1px solid rgba(251,191,36,0.4); color: var(--amber-400); padding: 5px 12px; border-radius: var(--radius-pill); cursor: pointer; font-size: 0.75rem; font-family: var(--font-body);">⚙ ระบุ IP ของ ESP32</button>
    </div>
  </div>

  <!-- ═══ MOCK DATA BANNER ═══ -->
  <div class="mock-banner" id="mock-banner">
    <svg width="16" height="16" viewBox="0 0 24 24" fill="currentColor"><path d="M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm1 15h-2v-6h2v6zm0-8h-2V7h2v2z"/></svg>
    ⚙ แสดงข้อมูลจำลอง (Mock Data) — ESP32 ยังไม่ได้เชื่อมต่อ
  </div>

  <!-- ═══ SENSOR CARDS ═══ -->
  <div class="cards-grid" id="cards-grid">

    <!-- Moisture -->
    <div class="card" id="card-moisture" style="--card-accent: linear-gradient(90deg,#22c55e,#4ade80);">
      <div class="card-header">
        <span class="card-label">ความชื้น</span>
        <div class="card-icon" style="--icon-bg: rgba(34,197,94,0.15);">
          <svg viewBox="0 0 24 24" fill="none" stroke="#4ade80" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round">
            <path d="M12 2.69l5.66 5.66a8 8 0 1 1-11.31 0z"/>
          </svg>
        </div>
      </div>
      <div>
        <span class="card-value" id="val-moisture">--</span>
        <span class="card-unit">%</span>
      </div>
      <div class="moisture-bar-wrap">
        <div class="moisture-bar-track">
          <div class="moisture-bar-fill" id="bar-moisture" style="width: 0%;"></div>
        </div>
        <div class="moisture-labels">
          <span>0%</span>
          <span>50%</span>
          <span>100%</span>
        </div>
      </div>
      <div class="card-status ok" id="status-moisture">● ปกติ</div>
    </div>

    <!-- Temperature -->
    <div class="card" id="card-temp" style="--card-accent: linear-gradient(90deg,#f97316,#fbbf24);">
      <div class="card-header">
        <span class="card-label">อุณหภูมิ</span>
        <div class="card-icon" style="--icon-bg: rgba(249,115,22,0.15);">
          <svg viewBox="0 0 24 24" fill="none" stroke="#fb923c" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round">
            <path d="M14 14.76V3.5a2.5 2.5 0 0 0-5 0v11.26a4.5 4.5 0 1 0 5 0z"/>
          </svg>
        </div>
      </div>
      <div>
        <span class="card-value" id="val-temperature">--</span>
        <span class="card-unit">°C</span>
      </div>
      <div class="card-status ok" id="status-temperature">● ปกติ</div>
    </div>

    <!-- EC -->
    <div class="card" id="card-ec" style="--card-accent: linear-gradient(90deg,#06b6d4,#2dd4bf);">
      <div class="card-header">
        <span class="card-label">EC / ค่าการนำไฟฟ้า</span>
        <div class="card-icon" style="--icon-bg: rgba(6,182,212,0.15);">
          <svg viewBox="0 0 24 24" fill="none" stroke="#22d3ee" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round">
            <polyline points="13 2 3 14 12 14 11 22 21 10 12 10 13 2"/>
          </svg>
        </div>
      </div>
      <div>
        <span class="card-value" id="val-ec">--</span>
        <span class="card-unit">µS/cm</span>
      </div>
      <div class="card-status ok" id="status-ec">● ปกติ</div>
    </div>

    <!-- pH -->
    <div class="card" id="card-ph" style="--card-accent: linear-gradient(90deg,#a855f7,#ec4899);">
      <div class="card-header">
        <span class="card-label">pH ดิน</span>
        <div class="card-icon" style="--icon-bg: rgba(168,85,247,0.15);">
          <svg viewBox="0 0 24 24" fill="none" stroke="#c084fc" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round">
            <circle cx="12" cy="12" r="10"/>
            <path d="M8 12h8M12 8v8"/>
          </svg>
        </div>
      </div>
      <div>
        <span class="card-value" id="val-ph">--</span>
        <span class="card-unit">pH</span>
      </div>
      <div class="card-status ok" id="status-ph">● ปกติ</div>
    </div>

    <!-- NPK -->
    <div class="card npk-card">
      <div class="card-header">
        <span class="card-label">N · P · K (ธาตุอาหาร)</span>
        <div class="card-icon" style="--icon-bg: rgba(168,85,247,0.15);">
          <svg viewBox="0 0 24 24" fill="none" stroke="#c084fc" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round">
            <circle cx="12" cy="7" r="4"/>
            <path d="M5 21v-2a7 7 0 0 1 14 0v2"/>
          </svg>
        </div>
      </div>
      <div class="npk-bars">
        <div class="npk-row">
          <span class="npk-label n">N</span>
          <div class="npk-track"><div class="npk-fill n" id="bar-n" style="width:0%"></div></div>
          <span class="npk-val" id="val-n">-- <small>mg/kg</small></span>
        </div>
        <div class="npk-row">
          <span class="npk-label p">P</span>
          <div class="npk-track"><div class="npk-fill p" id="bar-p" style="width:0%"></div></div>
          <span class="npk-val" id="val-p">-- <small>mg/kg</small></span>
        </div>
        <div class="npk-row">
          <span class="npk-label k">K</span>
          <div class="npk-track"><div class="npk-fill k" id="bar-k" style="width:0%"></div></div>
          <span class="npk-val" id="val-k">-- <small>mg/kg</small></span>
        </div>
      </div>
      <div class="card-status ok" id="status-npk" style="margin-top:14px;">● ปกติ</div>
    </div>

  </div><!-- /cards-grid -->

  <!-- ═══ RECOMMENDATIONS ═══ -->
  <div class="section-title">
    <svg width="14" height="14" viewBox="0 0 24 24" fill="currentColor"><path d="M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm1 17h-2v-2h2v2zm2.07-7.75l-.9.92C13.45 12.9 13 13.5 13 15h-2v-.5c0-1.1.45-2.1 1.17-2.83l1.24-1.26c.37-.36.59-.86.59-1.41 0-1.1-.9-2-2-2s-2 .9-2 2H8c0-2.21 1.79-4 4-4s4 1.79 4 4c0 .88-.36 1.68-.93 2.25z"/></svg>
    คำแนะนำ
  </div>
  <div class="rec-grid" id="rec-grid">
    <div class="rec-item info">
      <span class="rec-icon">⏳</span>
      <div class="rec-content">
        <div class="rec-title">รอรับข้อมูล</div>
        <div class="rec-desc">กำลังดึงข้อมูลจากเซ็นเซอร์...</div>
      </div>
    </div>
  </div>

  <!-- ═══ FOOTER ═══ -->
  <footer class="footer">
    <p>Agriscan &copy; 2025 · RS485 Modbus 7-in-1 Soil Sensor</p>
    <div class="footer-controls">
      <button class="btn-refresh" id="btn-refresh" onclick="manualRefresh()" aria-label="รีเฟรชข้อมูล">
        <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
          <path d="M23 4v6h-6M1 20v-6h6"/>
          <path d="M3.51 9a9 9 0 0 1 14.85-3.36L23 10M1 14l4.64 4.36A9 9 0 0 0 20.49 15"/>
        </svg>
        รีเฟรช
      </button>
    </div>
  </footer>

</div><!-- /page-wrapper -->

<!-- ═══════════════════════════════════════════
     JavaScript
════════════════════════════════════════════ -->
<script>
'use strict';

// ─── Config ───────────────────────────────────────────────
const CONFIG = {
  apiUrl:       '/data',
  interval:     3000,
  maxRetry:     999,
  retryDelay:   3000,
  useMockOnFail: false   // ไม่มี mock data — แสดงข้อมูลจริงเท่านั้น
};

// ─── State ────────────────────────────────────────────────
let state = {
  data:       null,
  online:     false,
  isMock:     false,
  retryCount: 0,
  timer:      null,
};

// ─── DOM refs ─────────────────────────────────────────────
const $ = id => document.getElementById(id);

// ─── Fetch data ───────────────────────────────────────────
async function fetchData() {
  try {
    const ctrl = new AbortController();
    const timeout = setTimeout(() => ctrl.abort(), 4000);
    const res = await fetch(CONFIG.apiUrl, { signal: ctrl.signal });
    clearTimeout(timeout);

    if (!res.ok) throw new Error(`HTTP ${res.status}`);
    const json = await res.json();

    state.online = true;
    state.isMock  = false;
    state.retryCount = 0;
    updateUI(json);
    setStatus('online');
    hideBanners();
  } catch (err) {
    state.online = false;
    state.retryCount++;

    setStatus('offline');
    showBanners();
  }
}

// ─── Update UI ────────────────────────────────────────────
function updateUI(d) {
  state.data = d;

  // Moisture
  const m = clamp(d.moisture, 0, 100);
  setValue('val-moisture', m.toFixed(1));
  animateBar('bar-moisture', m, 100);
  const barEl = $('bar-moisture');
  if (m < 30) {
    barEl.style.setProperty('--bar-color', '#f87171');
    barEl.style.background = 'linear-gradient(90deg,#ef4444,#f87171)';
    barEl.style.boxShadow  = '0 0 8px rgba(239,68,68,0.5)';
    setChip('status-moisture', 'alert', '⚠ ดินแห้ง — ควรรดน้ำ');
  } else if (m > 80) {
    barEl.style.background = 'linear-gradient(90deg,#3b82f6,#60a5fa)';
    barEl.style.boxShadow  = '0 0 8px rgba(59,130,246,0.5)';
    setChip('status-moisture', 'warn', '💧 ชื้นเกินไป');
  } else {
    barEl.style.background = 'linear-gradient(90deg,#22c55e,#4ade80)';
    barEl.style.boxShadow  = '0 0 8px rgba(34,197,94,0.5)';
    setChip('status-moisture', 'ok', '✓ ปกติ');
  }

  // Temperature
  setValue('val-temperature', (+d.temperature).toFixed(1));
  if (d.temperature > 35)      setChip('status-temperature', 'alert', '🌡 ร้อนมาก');
  else if (d.temperature < 15) setChip('status-temperature', 'warn', '❄ เย็นเกินไป');
  else                         setChip('status-temperature', 'ok', '✓ ปกติ');

  // EC
  setValue('val-ec', Math.round(d.ec));
  if (d.ec > 2000)       setChip('status-ec', 'alert', '⚠ เกลือสูง');
  else if (d.ec < 100)   setChip('status-ec', 'warn', '↓ EC ต่ำ');
  else                   setChip('status-ec', 'ok', '✓ ปกติ');

  // pH
  setValue('val-ph', (+d.ph).toFixed(1));
  if (d.ph < 5.5)       setChip('status-ph', 'alert', '⚠ กรดมาก');
  else if (d.ph > 7.5)  setChip('status-ph', 'warn', '⚠ ด่างมาก');
  else                  setChip('status-ph', 'ok', '✓ ปกติ');

  // NPK — เกณฑ์กรมพัฒนาที่ดิน (ตารางที่ 15, กองสำรวจดิน 2523)
  // P: ต่ำ <10, ปานกลาง 10–25, สูง >25 mg/kg · K: ต่ำ <60, ปานกลาง 60–90, สูง >90 mg/kg
  // N: กรมฯ ไม่มีเกณฑ์ mg/kg — เป็นค่าประมาณจากเซ็นเซอร์ (ใช้ 50 เป็นค่าบ่งชี้)
  const nMax = 200, pMax = 50, kMax = 150;
  setValue('val-n', Math.round(d.n) + ' <small>mg/kg</small>');
  setValue('val-p', Math.round(d.p) + ' <small>mg/kg</small>');
  setValue('val-k', Math.round(d.k) + ' <small>mg/kg</small>');
  animateBar('bar-n', d.n, nMax);
  animateBar('bar-p', d.p, pMax);
  animateBar('bar-k', d.k, kMax);

  const npkAlerts = [], npkWarns = [];
  if (d.n < 50)      npkAlerts.push('N ต่ำ');
  if (d.p < 10)      npkAlerts.push('P ต่ำ');
  else if (d.p < 25) npkWarns.push('P ปานกลาง');
  if (d.k < 60)      npkAlerts.push('K ต่ำ');
  else if (d.k < 90) npkWarns.push('K ปานกลาง');
  if (npkAlerts.length > 0)
    setChip('status-npk', 'warn', '⚠ ' + npkAlerts.join(' / '));
  else if (npkWarns.length > 0)
    setChip('status-npk', 'warn', 'ℹ ' + npkWarns.join(' / '));
  else
    setChip('status-npk', 'ok', '✓ ปกติ');

  // Recommendations
  buildRecommendations(d);

  // Timestamp — แสดงเวลาที่เซ็นเซอร์ส่งค่า (จากคลาวด์) ถ้า API ให้มา
  const ts = d.timestamp ? new Date(d.timestamp) : new Date();
  $('last-update').textContent = ts.toLocaleTimeString('th-TH');
}

// ─── Build recommendations ────────────────────────────────
function buildRecommendations(d) {
  const recs = [];

  if (d.moisture < 30) {
    recs.push({ type:'alert', icon:'💧', title:'ดินแห้ง — ควรรดน้ำ', desc:`ความชื้นปัจจุบัน ${d.moisture.toFixed(1)}% ต่ำกว่าเกณฑ์ ควรเปิดระบบรดน้ำทันที` });
  } else if (d.moisture > 80) {
    recs.push({ type:'warn', icon:'🌊', title:'ดินชื้นเกินไป', desc:`ความชื้น ${d.moisture.toFixed(1)}% สูงเกินไป อาจทำให้รากเน่าได้ ควรหยุดรดน้ำ` });
  } else {
    recs.push({ type:'ok', icon:'✅', title:'ความชื้นอยู่ในเกณฑ์ปกติ', desc:`ความชื้น ${d.moisture.toFixed(1)}% อยู่ในช่วงเหมาะสม (30–80%)` });
  }

  if (d.ph < 5.5) {
    recs.push({ type:'alert', icon:'🪨', title:'ดินเป็นกรดมาก', desc:`pH ${(+d.ph).toFixed(1)} ต่ำกว่าเกณฑ์ ควรใส่ปูนขาว (Lime) เพื่อปรับสภาพดิน` });
  } else if (d.ph > 7.5) {
    recs.push({ type:'warn', icon:'⚗️', title:'ดินเป็นด่าง', desc:`pH ${(+d.ph).toFixed(1)} สูงเกินไป ควรใส่กำมะถัน (Sulfur) เพื่อลด pH` });
  } else {
    recs.push({ type:'ok', icon:'✅', title:'pH อยู่ในเกณฑ์เหมาะสม', desc:`pH ${(+d.ph).toFixed(1)} อยู่ในช่วงเหมาะสม (5.5–7.5) สำหรับพืชส่วนใหญ่` });
  }

  // เกณฑ์ P, K อ้างอิงกรมพัฒนาที่ดิน (ตารางที่ 15) · N เป็นค่าประมาณจากเซ็นเซอร์
  if (d.n < 50) {
    recs.push({ type:'warn', icon:'🌿', title:'ไนโตรเจน (N) ค่อนข้างต่ำ', desc:`N = ${Math.round(d.n)} mg/kg (ค่าประมาณจากเซ็นเซอร์ — กรมฯ วัด N เป็น %) ควรใส่ปุ๋ยสูตรเน้น N เช่น 46-0-0 หรือ 21-0-0` });
  }

  if (d.p < 10) {
    recs.push({ type:'alert', icon:'🌱', title:'ฟอสฟอรัส (P) ต่ำ', desc:`P = ${Math.round(d.p)} mg/kg ต่ำกว่าเกณฑ์กรมพัฒนาที่ดิน (<10) ควรใส่ปุ๋ยสูตรเน้น P เช่น 0-46-0 หรือหินฟอสเฟต` });
  } else if (d.p < 25) {
    recs.push({ type:'warn', icon:'🌱', title:'ฟอสฟอรัส (P) ปานกลาง', desc:`P = ${Math.round(d.p)} mg/kg ระดับปานกลาง (10–25) ตามเกณฑ์กรมพัฒนาที่ดิน — ยังไม่ต้องใส่ปุ๋ย` });
  }

  if (d.k < 60) {
    recs.push({ type:'alert', icon:'🍂', title:'โพแทสเซียม (K) ต่ำ', desc:`K = ${Math.round(d.k)} mg/kg ต่ำกว่าเกณฑ์กรมพัฒนาที่ดิน (<60) ควรใส่ปุ๋ยสูตรเน้น K เช่น 0-0-60 หรือโพแทสเซียมคลอไรด์` });
  } else if (d.k < 90) {
    recs.push({ type:'warn', icon:'🍂', title:'โพแทสเซียม (K) ปานกลาง', desc:`K = ${Math.round(d.k)} mg/kg ระดับปานกลาง (60–90) ตามเกณฑ์กรมพัฒนาที่ดิน — ยังไม่ต้องใส่ปุ๋ย` });
  }

  if (d.ec > 2000) {
    recs.push({ type:'alert', icon:'⚡', title:'เริ่มเป็นดินเค็ม — ระวังการใส่ปุ๋ย', desc:`EC = ${Math.round(d.ec)} µS/cm เกิน 2,000 (2 dS/m) = เริ่มเค็มเล็กน้อยตามเกณฑ์กรมพัฒนาที่ดิน ควรระวังการใส่ปุ๋ยเพิ่ม` });
  }

  if (d.temperature > 35) {
    recs.push({ type:'alert', icon:'🌡', title:'อุณหภูมิดินสูงเกินไป', desc:`${(+d.temperature).toFixed(1)}°C อาจส่งผลต่อการดูดซึมของราก ควรคลุมดินเพื่อลดความร้อน` });
  }

  const grid = $('rec-grid');
  grid.innerHTML = recs.map(r => `
    <div class="rec-item ${r.type}">
      <span class="rec-icon">${r.icon}</span>
      <div class="rec-content">
        <div class="rec-title">${r.title}</div>
        <div class="rec-desc">${r.desc}</div>
      </div>
    </div>
  `).join('');
}

// ─── Helpers ──────────────────────────────────────────────
function setValue(id, val) {
  const el = $(id);
  if (!el) return;
  const old = el.innerHTML;
  el.innerHTML = String(val);
  if (old !== String(val) && old !== '--') {
    el.classList.remove('flash');
    void el.offsetWidth; // reflow
    el.classList.add('flash');
  }
}

function animateBar(id, val, max) {
  const el = $(id);
  if (!el) return;
  const pct = clamp((val / max) * 100, 0, 100);
  el.style.width = pct + '%';
}

function setChip(id, type, text) {
  const el = $(id);
  if (!el) return;
  el.className = `card-status ${type}`;
  el.textContent = text;
}

function setStatus(mode) {
  const badge = $('status-badge');
  const text  = $('status-text');
  badge.className = `status-badge ${mode}`;
  if (mode === 'online') {
    text.textContent = 'ออนไลน์';
  } else if (mode === 'offline') {
    text.textContent = state.isMock ? 'ออฟไลน์ (Mock)' : 'ออฟไลน์';
  } else {
    text.textContent = 'กำลังเชื่อมต่อ...';
  }
}

function showBanners() {
  $('connecting-banner').classList.add('visible');
  $('retry-count').textContent = `Retry ครั้งที่ ${state.retryCount} · ทุก ${CONFIG.retryDelay/1000} วินาที`;
  if (state.isMock) $('mock-banner').classList.add('visible');
}

function hideBanners() {
  $('connecting-banner').classList.remove('visible');
  $('mock-banner').classList.remove('visible');
}

function clamp(v, lo, hi) { return Math.min(hi, Math.max(lo, v)); }

// ─── Manual refresh ───────────────────────────────────────
async function manualRefresh() {
  const btn = $('btn-refresh');
  btn.classList.add('spinning');
  btn.disabled = true;
  await fetchData();
  setTimeout(() => {
    btn.classList.remove('spinning');
    btn.disabled = false;
  }, 600);
}

// ─── Auto-polling ─────────────────────────────────────────
function startPolling() {
  fetchData();
  state.timer = setInterval(fetchData, CONFIG.interval);
}

// ─── Init ─────────────────────────────────────────────────
window.addEventListener('DOMContentLoaded', () => {
  $('ip-badge').textContent = window.location.hostname || 'ESP32';
  startPolling();
});

// Reconnect when tab becomes visible
document.addEventListener('visibilitychange', () => {
  if (!document.hidden && !state.online) fetchData();
});
</script>
</body>
</html>
)rawliteral";
