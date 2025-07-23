import { Grid, showToast } from "@omnicast/api"
import { useEffect, useState } from "react";

const BASE_URL = "https://peepobank.com";

type Peepo = {
	name: string;
	filename: string;
	id: string;
	url: string;
};

const fetchRandomPage = async (n = 50) => {
	const res = await fetch(`${BASE_URL}/api/random?n=${n}`)
	const json = await res.json();

	return json.map((obj: any) => {
		return {
			name: obj.name,
			filename: obj.filename,
			id: obj.id,
			url: `${BASE_URL}/cdn${obj.filename}`
		};
	});
}

const FruitGrid = () => {
	const [isLoading, setIsLoading] = useState(false);
	const [peepos, setPeepos] = useState<Peepo[]>([]);

	const startSearch = (query: string) => {
		console.log('search', query);
		setIsLoading(true);
		fetchRandomPage()
		.then((peepos) => { setPeepos(peepos); console.log(`fetched peepos ${peepos.length}`)})
		.catch(error => { console.error(error) })
		.finally(() => { setIsLoading(false); });
	}

	useEffect(() => {
		startSearch("");
	}, []);

	useEffect(() => {
		console.error('peepos updated', peepos);
	}, [peepos]);

	return (
		<Grid 
			isLoading={isLoading}
			searchBarPlaceholder={'Search for peepo'}
			onSearchTextChange={startSearch}
			onSelectionChange={() => {}}
		>
			<Grid.Section inset={20} title="Results" columns={6}>
				{peepos.map(peepo => (
					<Grid.Item title={peepo.name} subtitle={peepo.id} key={peepo.id} id={peepo.id} content={peepo.url} />
				))}
			</Grid.Section>
		</Grid>
	);
}

const ExampleCommand = () => {
	useEffect(() => {
		showToast({ title: 'Hello from useEffect' });
	}, []);

	return <FruitGrid />
}

export default ExampleCommand;
