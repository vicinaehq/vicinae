import glob from 'glob';
import Jasmine from 'jasmine';

const jasmine = new Jasmine();
jasmine.loadConfigFile('test/jasmine.json');

const pattern = process.argv[2] || 'test/spec/*.js';

// Load your specs
glob(pattern, function (er, files) {
    Promise.all(
        files
            // Use relative paths
            .map(f => f.replace(/^([^\/])/, './$1'))
            .map(f => import(f)
                .catch(e => {
                    console.error('** Error loading ' + f + ': ');
                    console.error(e);
                    process.exit(1);
                }))
    )
    .then(() => jasmine.execute());
});