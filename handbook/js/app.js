const search = document.querySelector('#search');
const cards = [...document.querySelectorAll('.card')];
const noResults = document.querySelector('.no-results');
let activeTag = '';
function filter(){
  const terms = search.value.toLowerCase().trim().split(/\s+/).filter(Boolean);
  let shown=0;
  for(const card of cards){
    const hay=(card.innerText+' '+(card.dataset.tags||'')).toLowerCase();
    const ok=terms.every(t=>hay.includes(t)) && (!activeTag||(card.dataset.tags||'').split(' ').includes(activeTag));
    card.classList.toggle('hidden',!ok); if(ok)shown++;
  }
  noResults.style.display=shown?'none':'block';
}
search.addEventListener('input',filter);
document.querySelectorAll('.tag-filter').forEach(button=>button.addEventListener('click',()=>{
  activeTag=activeTag===button.dataset.tag?'':button.dataset.tag;
  document.querySelectorAll('.tag-filter').forEach(x=>x.classList.toggle('active',x.dataset.tag===activeTag)); filter();
}));
cards.forEach(card=>{
  const button=document.createElement('button'); button.className='collapse-button'; button.textContent='折りたたむ';
  button.addEventListener('click',()=>{card.classList.toggle('collapsed');button.textContent=card.classList.contains('collapsed')?'開く':'折りたたむ';});
  card.prepend(button);
});
document.addEventListener('keydown',e=>{if(e.key==='/'&&document.activeElement!==search){e.preventDefault();search.focus();}});
