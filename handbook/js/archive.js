(() => {
  "use strict";
  const data = window.HT_AHC_ARCHIVE;
  const list = document.getElementById("archive-list");
  const search = document.getElementById("archive-search");
  const buttons = [...document.querySelectorAll("[data-archive-filter]")];
  let filter = "all";
  const pad = n => String(n).padStart(3,"0");
  const esc = v => String(v).replace(/[&<>"']/g,c=>({"&":"&amp;","<":"&lt;",">":"&gt;","\"":"&quot;","'":"&#39;"})[c]);
  const url = (base, query) => `${base}${encodeURIComponent(query)}`;

  const cards = [];
  for (let n = data.latestPast; n >= 1; --n) {
    const id = `ahc${pad(n)}`;
    const spotlight = data.spotlights[n];
    const title = spotlight?.title || `AtCoder Heuristic Contest ${pad(n)}`;
    const tags = spotlight?.tags || [];
    const searchText = [id,title,...tags,spotlight?.note||""].join(" ").toLowerCase();
    cards.push(`<article class="archive-card ${spotlight ? "spotlight":""}" data-spotlight="${spotlight ? "yes":"no"}" data-tags="${esc(tags.join(" "))}" data-search="${esc(searchText)}">
      <div class="archive-title"><span class="archive-number">${id.toUpperCase()}</span><h2>${esc(title)}</h2></div>
      ${spotlight ? `<p>${esc(spotlight.note)}</p><div class="archive-tags">${tags.map(t=>`<span>${esc(t)}</span>`).join("")}</div>` : `<p>公式資料と公開された振り返りへ直接移動するための索引です。</p>`}
      <div class="archive-links">
        <a href="https://atcoder.jp/contests/${id}/tasks" target="_blank" rel="noreferrer">問題</a>
        <a href="https://atcoder.jp/contests/${id}/editorial" target="_blank" rel="noreferrer">公式・ユーザー解説</a>
        <a href="https://atcoder.jp/contests/${id}/standings" target="_blank" rel="noreferrer">順位表</a>
        <a href="${url("https://www.google.com/search?q=", `${id.toUpperCase()} 参加記 解法`)}" target="_blank" rel="noreferrer">参加記検索</a>
        <a href="${url("https://x.com/search?q=", `#${id.toUpperCase()}`)}&src=typed_query&f=live" target="_blank" rel="noreferrer">感想 #${id.toUpperCase()}</a>
        <a href="${url("https://www.youtube.com/results?search_query=", `${id.toUpperCase()} AHCラジオ wata`)}" target="_blank" rel="noreferrer">AHCラジオ</a>
      </div>
    </article>`);
  }
  list.innerHTML = cards.join("");
  document.getElementById("archive-count").textContent = String(data.latestPast);
  document.getElementById("archive-checked").textContent = data.checked;

  function update() {
    const terms = search.value.trim().toLowerCase().split(/\s+/).filter(Boolean);
    let visible = 0;
    document.querySelectorAll(".archive-card").forEach(card => {
      const filterMatch = filter === "all" || (filter === "spotlight" && card.dataset.spotlight === "yes") || card.dataset.tags.split(" ").includes(filter);
      const queryMatch = terms.every(term => card.dataset.search.includes(term));
      const show = filterMatch && queryMatch;
      card.classList.toggle("hidden", !show); if (show) visible++;
    });
    document.getElementById("archive-empty").classList.toggle("visible", visible === 0);
  }
  search.addEventListener("input", update);
  buttons.forEach(button => button.addEventListener("click", () => {
    filter = button.dataset.archiveFilter;
    buttons.forEach(x => x.setAttribute("aria-pressed", String(x===button)));
    update();
  }));
})();
