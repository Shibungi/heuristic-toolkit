(() => {
  "use strict";
  const root = document.getElementById("technique-list");
  const search = document.getElementById("technique-search");
  const filters = [...document.querySelectorAll("[data-tech-filter]")];
  const techniques = window.HT_TECHNIQUES || [];
  const escape = (v) => String(v).replace(/[&<>"']/g, c => ({"&":"&amp;","<":"&lt;",">":"&gt;","\"":"&quot;","'":"&#39;"})[c]);
  let category = "all";

  const html = techniques.map((t) => `
    <article id="${escape(t.id)}" class="technique-card" data-category="${escape(t.category)}" data-search="${escape([t.title,t.aliases,t.summary,t.trigger,t.contests].join(" ").toLowerCase())}">
      <header><div><p class="tech-category">${escape(t.category)}</p><h2>${escape(t.title)}</h2></div><p>${escape(t.summary)}</p></header>
      <div class="tech-grid"><section><h3>使う兆候</h3><p>${escape(t.trigger)}</p></section><section><h3>計算量・費用</h3><p>${escape(t.cost)}</p></section></div>
      <details><summary>実装手順</summary><ol>${t.recipe.map(x=>`<li>${escape(x)}</li>`).join("")}</ol></details>
      <details><summary>落とし穴・過去例</summary><p class="warning">${escape(t.danger)}</p><p><strong>関連:</strong> ${escape(t.contests)}</p>${t.sources.length ? `<p class="source-links">${t.sources.map(([label,url])=>`<a href="${escape(url)}" target="_blank" rel="noreferrer">${escape(label)}</a>`).join("")}</p>`:""}</details>
    </article>`).join("");
  root.innerHTML = html;
  document.getElementById("technique-count").textContent = String(techniques.length);

  function update() {
    const terms = search.value.trim().toLowerCase().split(/\s+/).filter(Boolean);
    let visible = 0;
    document.querySelectorAll(".technique-card").forEach(card => {
      const show = (category === "all" || card.dataset.category === category) && terms.every(term => card.dataset.search.includes(term) || card.textContent.toLowerCase().includes(term));
      card.classList.toggle("hidden", !show); if (show) visible++;
    });
    document.getElementById("technique-empty").classList.toggle("visible", visible === 0);
  }
  search.addEventListener("input", update);
  filters.forEach(button => button.addEventListener("click", () => {
    category = button.dataset.techFilter;
    filters.forEach(x => x.setAttribute("aria-pressed", String(x===button)));
    update();
  }));
})();
