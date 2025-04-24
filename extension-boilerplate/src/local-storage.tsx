import { Action, ActionPanel, Icon, List, LocalStorage, clearSearchBar, getPreferenceValues } from '@omnicast/api';
import { randomUUID } from 'crypto';
import React, { useEffect, useState } from 'react';

export const LocalStorageList = () => {
	const [items, setItems] = useState<LocalStorage.Values>({});

	useEffect(() => {
		refetchStorage();

		console.log({ preferences: getPreferenceValues() });
	}, []);


	const refetchStorage = async () => {
		setItems(await LocalStorage.allItems());
	}

	const clearItem = async (key: string) => {
		await LocalStorage.removeItem(key);
		await refetchStorage();
	}

	const addRandomItem = async () => {
		const key = randomUUID();

		await LocalStorage.setItem(key, randomUUID());
		await refetchStorage();
		clearSearchBar();
	}

	return (
		<List
			isShowingDetail
			searchBarPlaceholder={'Browse local storage'}
			actions={
				<ActionPanel>
					<Action title="Add random item" icon={Icon.PlusCircle} onAction={addRandomItem} />
				</ActionPanel>
			}
		>
			{Object.entries(items).map(([k, v]) => (
				<List.Item 
					key={k}
					title={k}
					icon={Icon.CodeBlock}
					actions={
						<ActionPanel>
							<Action title="Clear item" icon={Icon.Trash} onAction={() => clearItem(k)} />
						</ActionPanel>
					}
					detail={
						<List.Item.Detail markdown={`${v}`} />
					}
				/>
			))}
		</List>
	);
};

export default LocalStorageList;
