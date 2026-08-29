(() => {
  "use strict";
  const data = window.HT_LIBRARY_API || {};
  const concepts = window.HT_LIBRARY_CONCEPTS || {};
  const escape = (value) => String(value).replace(/[&<>"']/g, (ch) => ({"&":"&amp;","<":"&lt;",">":"&gt;","\"":"&quot;","'":"&#39;"})[ch]);
  let count = 0;

  Object.entries(data).forEach(([id, guide]) => {
    const entry = document.getElementById(id);
    if (!entry) return;
    count += guide.methods.length;
    const concept = concepts[id];
    if (concept) {
      const panel = document.createElement("section");
      panel.className = "concept-contract";
      panel.innerHTML = `
        <h4>このライブラリが行うこと</h4>
        <dl class="concept-grid">
          <div class="concept-objective"><dt>目的・比較方向</dt><dd>${escape(concept.objective)}</dd></div>
          <div><dt>保持する状態</dt><dd>${escape(concept.state)}</dd></div>
          <div><dt>1ステップ</dt><dd>${escape(concept.transition)}</dd></div>
          <div><dt>採否・選択</dt><dd>${escape(concept.decision)}</dd></div>
          <div><dt>返すもの</dt><dd>${escape(concept.output)}</dd></div>
          <div><dt>向く場面</dt><dd>${escape(concept.use)}</dd></div>
        </dl>
        <p class="concept-trap"><strong>取り違えやすい点:</strong> ${escape(concept.trap)}</p>`;
      const anchor = entry.querySelector(".entry-facts") || entry.querySelector(".entry-summary") || entry.firstElementChild;
      anchor.insertAdjacentElement("afterend", panel);
    }
    const details = document.createElement("details");
    details.className = "api-reference";
    const methods = guide.methods.map((item) => `
      <section class="api-method">
        <h4><code>${escape(item.signature)}</code></h4>
        <p>${escape(item.description)}</p>
        <dl class="api-meta">
          <div><dt>引数・契約</dt><dd>${escape(item.params)}</dd></div>
          <div><dt>戻り値・変更</dt><dd>${escape(item.returns)}</dd></div>
          <div><dt>計算量</dt><dd>${escape(item.complexity)}</dd></div>
        </dl>
        ${item.notes ? `<p class="api-note"><strong>注意:</strong> ${escape(item.notes)}</p>` : ""}
      </section>`).join("");
    details.innerHTML = `<summary>公開API・各メソッド（${guide.methods.length}）</summary><p class="api-design">${escape(guide.design)}</p><div class="api-methods">${methods}</div>`;
    entry.append(details);
  });

  const stat = document.getElementById("api-count");
  if (stat) stat.textContent = String(count);
})();
