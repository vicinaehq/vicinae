import { Action, ActionPanel, AI, getPreferenceValues, Icon, List } from "@omnicast/api"
import { useEffect, useState } from "react";
import { Fruit, fruits } from "./fruits";


const FruitList = () => {
	const [isLoading, setIsLoading] = useState(false);

	const handleCustomCallback = (fruit: Fruit) => {
		console.log('custom callback fired with', fruit);
	}

	const handleGenerateAI = async () => {
		setIsLoading(true);
		const stream = AI.ask(`Tell me who you are`);

		stream.on('data', (token) => {
			console.log({ token });
		});

		stream.finally(() => setIsLoading(false));
	}

	useEffect(() => {
		console.log({ preferences: getPreferenceValues() });
	}, []);

	return (
		<List isShowingDetail isLoading={isLoading} searchBarPlaceholder={'Search for a fruit'}>
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
								<Action title="Generate AI text" icon={Icon.Dna} onAction={() => handleGenerateAI() } />
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
