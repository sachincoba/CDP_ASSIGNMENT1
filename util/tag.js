const glob = require('glob');
const fs = require('fs');

// look for all .md files in the given location.
const files = glob.sync(`./website/docs/*.md`);

for (const file of files) {
  const content = fs.readFileSync(file, 'utf8');
  const updated = content.replace(/<!-- -->/g, '');
  fs.writeFileSync(file, updated);
}
