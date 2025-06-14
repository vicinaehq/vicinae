import { Grid, showToast } from "@omnicast/api"
import { useEffect, useState } from "react";

const BASE_URL = "https://peepobank.com";

type Peepo = {
	name: string;
	filename: string;
	id: string;
	url: string;
};

const fetchRandomPage = async (n = 20) => {
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
		setIsLoading(true);
		fetchRandomPage()
		.then((peepos) => { setPeepos(peepos); console.log(`fetched peepos ${peepos.length}`)})
		.catch(error => {})
		.finally(() => { setIsLoading(false); });
	}

	useEffect(() => {
		startSearch("");
	}, []);

	useEffect(() => {
		console.log('peepos updated', peepos);
	}, [peepos]);

	return (
		<Grid 
			isLoading={isLoading}
			searchBarPlaceholder={'Search for peepo'}
			onSearchTextChange={startSearch}
			onSelectionChange={() => {}}
		>
			<Grid.Section title="Results" columns={8}>
				{peepos.map(peepo => (
					<Grid.Item key={peepo.id} id={peepo.id} content={peepo.url} />
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
