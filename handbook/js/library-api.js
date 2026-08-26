(() => {
  "use strict";
  const data = window.HT_LIBRARY_API || {};
  const escape = (value) => String(value).replace(/[&<>"']/g, (ch) => ({"&":"&amp;","<":"&lt;",">":"&gt;","\"":"&quot;","'":"&#39;"})[ch]);
  let count = 0;

  Object.entries(data).forEach(([id, guide]) => {
    const entry = document.getElementById(id);
    if (!entry) return;
    count += guide.methods.length;
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
