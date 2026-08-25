(() => {
  "use strict";

  const entries = [...document.querySelectorAll(".library-entry")];
  const sections = [...document.querySelectorAll(".catalog-section")];
  const search = document.getElementById("library-search");
  const noResults = document.getElementById("no-library-results");
  const filterButtons = [...document.querySelectorAll("#category-filters [data-category]")];
  let activeCategory = "all";

  const searchableText = (entry) => `${entry.dataset.search || ""} ${entry.textContent}`.toLocaleLowerCase("ja");
  const index = new Map(entries.map((entry) => [entry, searchableText(entry)]));

  function updateCatalog() {
    const terms = search.value.trim().toLocaleLowerCase("ja").split(/\s+/).filter(Boolean);
    let visible = 0;
    entries.forEach((entry) => {
      const categoryMatches = activeCategory === "all" || entry.dataset.category === activeCategory;
      const haystack = index.get(entry);
      const searchMatches = terms.every((term) => haystack.includes(term));
      const show = categoryMatches && searchMatches;
      entry.classList.toggle("hidden", !show);
      if (show) visible += 1;
    });
    sections.forEach((section) => {
      section.classList.toggle("hidden", !section.querySelector(".library-entry:not(.hidden)"));
    });
    noResults.classList.toggle("visible", visible === 0);
  }

  search.addEventListener("input", updateCatalog);
  filterButtons.forEach((button) => {
    button.setAttribute("aria-pressed", button.dataset.category === "all" ? "true" : "false");
    button.addEventListener("click", () => {
      activeCategory = button.dataset.category;
      filterButtons.forEach((item) => {
        const selected = item === button;
        item.classList.toggle("active", selected);
        item.setAttribute("aria-pressed", String(selected));
      });
      updateCatalog();
    });
  });
  document.addEventListener("keydown", (event) => {
    if (event.key === "/" && !event.ctrlKey && !event.metaKey && !event.altKey && !/INPUT|TEXTAREA|SELECT/.test(document.activeElement.tagName)) {
      event.preventDefault();
      search.focus();
    }
  });

  const expandButton = document.getElementById("expand-all");
  expandButton.addEventListener("click", () => {
    const visibleDetails = entries
      .filter((entry) => !entry.classList.contains("hidden"))
      .flatMap((entry) => [...entry.querySelectorAll("details")]);
    const shouldOpen = visibleDetails.some((detail) => !detail.open);
    visibleDetails.forEach((detail) => { detail.open = shouldOpen; });
    expandButton.textContent = shouldOpen ? "すべて閉じる" : "すべて開く";
  });

  function copyFallback(text) {
    const area = document.createElement("textarea");
    area.value = text;
    area.style.position = "fixed";
    area.style.opacity = "0";
    document.body.append(area);
    area.select();
    document.execCommand("copy");
    area.remove();
  }

  document.querySelectorAll("pre").forEach((pre) => {
    const code = pre.querySelector("code");
    if (!code) return;
    const button = document.createElement("button");
    button.type = "button";
    button.className = "copy-code";
    button.textContent = "コピー";
    button.setAttribute("aria-label", "コードをコピー");
    button.addEventListener("click", async () => {
      try {
        if (navigator.clipboard && window.isSecureContext) await navigator.clipboard.writeText(code.textContent);
        else copyFallback(code.textContent);
        button.textContent = "コピー済み";
      } catch (_) {
        button.textContent = "コピー失敗";
      }
      window.setTimeout(() => { button.textContent = "コピー"; }, 1400);
    });
    pre.prepend(button);
  });

  const navLinks = [...document.querySelectorAll("#guide-nav a[href^='#']")];
  const navTargets = navLinks.map((link) => document.querySelector(link.getAttribute("href"))).filter(Boolean);
  if ("IntersectionObserver" in window) {
    const observer = new IntersectionObserver((observations) => {
      const candidate = observations.filter((item) => item.isIntersecting).sort((a, b) => a.boundingClientRect.top - b.boundingClientRect.top)[0];
      if (!candidate) return;
      navLinks.forEach((link) => link.classList.toggle("active", link.getAttribute("href") === `#${candidate.target.id}`));
    }, { rootMargin: "-18% 0px -70% 0px", threshold: 0 });
    navTargets.forEach((target) => observer.observe(target));
  }

  const beamEvents = [
    { code: "root", node: "root", edges: [], detail: "可変Stateをrootに初期化します。", leaf: null },
    { code: "apply(a)", node: "a", edge: "root-a", edges: ["root-a"], detail: "下降辺aを適用。Stateはroot→aへ進みます。", leaf: null },
    { code: "apply(c)", node: "c", edge: "a-c", edges: ["root-a", "a-c"], detail: "Action cを適用し、候補Cの状態をその場で作ります。", leaf: "c" },
    { code: "expand C", node: "c", edges: ["root-a", "a-c"], detail: "Cから次の候補を列挙。Stateコピーは保存しません。", leaf: "c" },
    { code: "undo(c)", node: "a", edge: "a-c", edges: ["root-a"], detail: "cの変更だけを戻し、共有prefix aへ復帰します。", leaf: null },
    { code: "apply(d)", node: "d", edge: "a-d", edges: ["root-a", "a-d"], detail: "兄弟Dへ移動。prefix aは作り直しません。", leaf: "d" },
    { code: "expand D", node: "d", edges: ["root-a", "a-d"], detail: "同じ1個のStateでDを評価します。", leaf: "d" },
    { code: "undo(d)", node: "a", edge: "a-d", edges: ["root-a"], detail: "dをundoしてaへ戻ります。", leaf: null },
    { code: "undo(a)", node: "root", edge: "root-a", edges: [], detail: "部分木aを出てrootへ戻ります。", leaf: null },
    { code: "apply(b)", node: "b", edge: "root-b", edges: ["root-b"], detail: "次の共有prefix bを適用します。", leaf: null },
    { code: "apply(e)", node: "e", edge: "b-e", edges: ["root-b", "b-e"], detail: "候補Eの状態を作ります。", leaf: "e" },
    { code: "expand E", node: "e", edges: ["root-b", "b-e"], detail: "Eを評価。通常BeamならC/D/Eの3 Stateを保持します。", leaf: "e" },
    { code: "undo(e)", node: "b", edge: "b-e", edges: ["root-b"], detail: "eをundoしてbへ戻ります。", leaf: null },
    { code: "undo(b)", node: "root", edge: "root-b", edges: [], detail: "Euler tourを完了し、Stateは初期値と一致します。", leaf: null }
  ];
  const positions = { root: [240, 40], a: [140, 105], b: [340, 105], c: [75, 185], d: [185, 185], e: [340, 185] };
  const beamStep = document.getElementById("beam-step");
  const beamPlay = document.getElementById("beam-play");
  const eventCode = document.getElementById("beam-event-code");
  const eventDetail = document.getElementById("beam-event-detail");
  const eventProgress = document.getElementById("beam-event-progress");
  const progressTrack = eventProgress.parentElement;
  const cursor = document.getElementById("state-cursor");
  const cursorLabel = document.getElementById("cursor-label");
  let beamIndex = 0;
  let playTimer = null;

  progressTrack.setAttribute("role", "progressbar");
  progressTrack.setAttribute("aria-label", "Euler tour event");
  progressTrack.setAttribute("aria-valuemin", "1");
  progressTrack.setAttribute("aria-valuemax", String(beamEvents.length));

  function renderBeamEvent() {
    const event = beamEvents[beamIndex];
    const [x, y] = positions[event.node];
    cursor.setAttribute("cx", String(x));
    cursor.setAttribute("cy", String(y - 27));
    cursorLabel.setAttribute("x", String(x + 11));
    cursorLabel.setAttribute("y", String(y - 28));
    document.querySelectorAll("[data-node]").forEach((node) => node.classList.toggle("active", node.dataset.node === event.node));
    document.querySelectorAll("[data-edge]").forEach((edge) => {
      edge.classList.toggle("visited", event.edges.includes(edge.dataset.edge));
      edge.classList.toggle("active", edge.dataset.edge === event.edge);
    });
    document.querySelectorAll("[data-normal-leaf]").forEach((leaf) => leaf.classList.toggle("active", leaf.dataset.normalLeaf === event.leaf));
    eventCode.textContent = event.code;
    eventDetail.textContent = event.detail;
    const progress = ((beamIndex + 1) / beamEvents.length) * 100;
    eventProgress.style.width = `${progress}%`;
    progressTrack.setAttribute("aria-valuenow", String(beamIndex + 1));
  }

  function advanceBeam() {
    beamIndex = (beamIndex + 1) % beamEvents.length;
    renderBeamEvent();
  }

  function stopBeam() {
    if (playTimer !== null) window.clearInterval(playTimer);
    playTimer = null;
    beamPlay.textContent = "再生";
    beamPlay.setAttribute("aria-pressed", "false");
  }

  beamStep.addEventListener("click", () => { stopBeam(); advanceBeam(); });
  beamPlay.addEventListener("click", () => {
    if (playTimer !== null) { stopBeam(); return; }
    beamPlay.textContent = "一時停止";
    beamPlay.setAttribute("aria-pressed", "true");
    playTimer = window.setInterval(advanceBeam, 900);
  });
  document.addEventListener("visibilitychange", () => { if (document.hidden) stopBeam(); });
  renderBeamEvent();
})();
