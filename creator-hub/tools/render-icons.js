const {chromium}=require('playwright');const fs=require('fs');
const src=fs.readFileSync(process.argv[2],'utf8');const out=process.argv[3];
const g=src.match(/const W='#fff';\s*const GLYPHS=\{[\s\S]*?\n\};/)[0];
const sw=src.match(/const SWATCHES=(\[[^\]]*\])/)[1];
(async()=>{const b=await chromium.launch();const p=await b.newPage({deviceScaleFactor:1});
await p.setContent('<body style="margin:0"></body>');
const list=await p.evaluate(([g,sw])=>{eval(g.replace('const W','window.W').replace('const GLYPHS','window.GLYPHS'));window.SW=eval(sw);
  return Object.keys(GLYPHS).flatMap(k=>SW.map(c=>[k,c]))},[g,sw]);
const sizes={'@2x':120,'@3x':180};
for(const [k,c] of list){for(const [suf,px] of Object.entries(sizes)){
  await p.setViewportSize({width:px,height:px});
  await p.evaluate(([k,c,px])=>{document.body.innerHTML=`<svg width="${px}" height="${px}" viewBox="0 0 24 24" style="display:block;color:${c}"><rect width="24" height="24" fill="${c}"/><g transform="translate(12 12) scale(1.18) translate(-12 -12.4)">${GLYPHS[k]}</g></svg>`},[k,c,px]);
  await p.screenshot({path:`${out}/${k}_${c.slice(1)}${suf}.png`,omitBackground:false});}}
// primary icon (iPhone + iPad sizes)
for(const [name,px] of [['AppIcon60x60@2x',120],['AppIcon60x60@3x',180],['AppIcon76x76~ipad',76],['AppIcon76x76@2x~ipad',152],['AppIcon83.5x83.5@2x~ipad',167]]){
  await p.setViewportSize({width:px,height:px});
  await p.evaluate(([px])=>{document.body.innerHTML=`<svg width="${px}" height="${px}" viewBox="0 0 24 24" style="display:block;color:#e3262d"><rect width="24" height="24" fill="#e3262d"/><g transform="translate(12 12) scale(1.18) translate(-12 -12.4)">${GLYPHS.lines}</g></svg>`},[px]);
  await p.screenshot({path:`${out}/${name}.png`});}
console.log(list.length,'alternate icons');await b.close()})();
