import { Action, ActionPanel, AI, getPreferenceValues, Icon, List } from "@omnicast/api"
import { useEffect, useRef, useState } from "react";
import { Fruit, fruits } from "./fruits";

const FruitGen = () => {
	const [isLoading, setIsLoading] = useState(false);
	const [generations, setGenerations] = useState<{[key: string]: string}>({});
	const abortController = useRef<AbortController>(new AbortController);
	const currentGen = useRef('');

	const handleGeneration = (fruit: Fruit) => {
		const stream = AI.ask(`Write an essay about this fruit: ${fruit.name}. Include images if possible`, { signal: abortController.current.signal });

		setIsLoading(true);
		currentGen.current = '';

		stream.on('data', (token) => {
			currentGen.current += token;
			setGenerations({
				...generations,
				[fruit.name]: currentGen.current
			});
		});

		stream.finally(() => setIsLoading(false));
	}

	useEffect(() => {
		return () => {
			abortController.current.abort();
		}
	}, []);

	useEffect(() => {
		console.log({generations});
	}, [generations]);

	return (
		<List isShowingDetail isLoading={isLoading} searchBarPlaceholder={'Search for a fruit'}>
			<List.Section title={"Fruits"}>
				{fruits.map((fruit) => (
					<List.Item 
						title={fruit.name}
						icon={fruit.emoji}
						key={fruit.name} 
						detail={generations[fruit.name] && <List.Item.Detail markdown={generations[fruit.name]} />}
						actions={
							<ActionPanel>
								<Action title="Generate AI" icon={Icon.Dna} onAction={() => handleGeneration(fruit)} />
							</ActionPanel>
						}
					/>
				))}
			</List.Section>
		</List>
	);
};

const FruitList = () => {
	const handleCustomCallback = (fruit: Fruit) => {
		console.log('custom callback fired with', fruit);
	}

	useEffect(() => {
		console.log({ preferences: getPreferenceValues() });
	}, []);

	return (
		<List isShowingDetail searchBarPlaceholder={'Search for a fruit'}>
			<List.Section title={"Fruits"}>
				{fruits.map(fruit => (
					<List.Item 
						title={fruit.name}
						icon={fruit.emoji}
						key={fruit.name} 
						detail={
							<List.Item.Detail markdown={fruit.description} />
						}
						actions={
							<ActionPanel>
								<Action.CopyToClipboard title={"Copy to clipboard"} content={fruit.emoji} />
								<Action title="Custom callback" icon={Icon.Pencil} onAction={() => handleCustomCallback(fruit)} />
								<Action.Push title="Switch to AI gen" icon={Icon.Dna} target={<FruitGen />} />
							</ActionPanel>
						}
					/>
				))}
			</List.Section>
		</List>
	);
}

const ExampleCommand = () => {
	return <FruitList />
}


export default ExampleCommand;
