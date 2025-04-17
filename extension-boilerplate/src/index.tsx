import { Action, ActionPanel, Detail, getPreferenceValues, Icon, List } from "@omnicast/api"
import { useEffect } from "react";
import { Fruit, fruits } from "./fruits";


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
