const { randomUUID } = require('crypto');
  // create a new one
const { createConnection } = require('net');

const writeData = (client, data) => {
	const databuf = Buffer.from(data, 'utf-8');
	const buf = Buffer.allocUnsafe(4);

	buf.writeUInt32LE(databuf.length, 0);

	return client.write(Buffer.concat([buf, databuf]));
}

const MyList = (n) => {
	const id1 = randomUUID();

	const render = () => {
		return ({
			type: "container",
			props: { 
				direction: 'vertical',
				margins: [10, 10, 10, 10],
			},
			children: [
				{
					type: "container",
					margins: [0, 0, 0, 0],
					props: { direction: 'horizontal' },
					children: [
						{
							type: "SearchInput",
							props: { 
								placeholder: "Shut the bing brag the ting",
								onTextChanged: randomUUID(),
								style: 'font-size: 16px',
							},
							children: []
						},
						{
							type: "Image",
							props: { 
								path: '/home/aurelle/Pictures/peepobank/ez.png',
								width: 30,
								height: 30
							},
							children: []
						},
					]
				},
				{
					type: "List",
					props: {},
					children: [
						{ type: "ListItem", props: { label: "TingTing", onClick: 'ereriueruerie' } },
						{ type: "ListItem", props: { label: `Count: ${n}` } },
						{ type: "ListItem", props: { label: "BingBing", onClick: 'wejkrikjejerijere' } }
					]
				},
			]
		});
	}

	return render();
}

const main = async () => {
	const client = createConnection({ path: process.env.ENDPOINT });

	client.on('error', (error) => console.log(error));

	console.log('starting bro');

	writeData(client, JSON.stringify({
		type: 'register',
		data: {
			token: process.env.TOKEN
		}
	}));
	writeData(client, JSON.stringify({ type: "render", data: { root: MyList(0) }}));

	let n = 0;

	client.on('data', (data) => {
		const s = data.subarray(4).toString('utf-8');
		n += 1;

		try {
		console.log(JSON.parse(s));
		} catch(error) {
			console.error(`Failed to parse json: ${error}`);
		}

		writeData(client, JSON.stringify({ type: "render", data: { root: MyList(n) }}));
	});

	while (true) {
		await new Promise((resolve => setTimeout(() => resolve(), 1000)));
		writeData(client, JSON.stringify({ type: "render", data: { root: MyList(Date.now()) }}));
	}
}

main();
