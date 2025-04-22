import { Ref } from 'react';
import { useImperativeFormHandle } from '../hooks/use-imperative-form-handle';

type FormProps = {
	actions?: React.ReactNode;
	children?: React.ReactNode
	enableDrafts?: boolean,
	isLoading?: boolean,
	navigationTitle?: string,
	searchBarAccessory?: React.ReactNode
};

export type FormItemRef = {
	focus: () => void;
	reset: () => void;
};

interface FormItemProps<T extends Form.Value> {
	id: string;
	title?: string;
	info?: string;
	error?: string;
	storeValue?: boolean;
	autoFocus?: boolean;
	value?: T;
	defaultValue?: T;
	onChange?: (newValue: T) => void;
	onBlur?: (event: FormEvent<T>) => void;
	onFocus?: (event: FormEvent<T>) => void;
};

interface FormEvent<T extends Form.Value> {
	target: {
		id: string,
		value?: T;
	};
	type: FormEventType;
};

type FormEventType = 'focus' | 'blur';

export declare namespace Form {
	export type Props = FormProps;
	export type TextField = FormItemRef
	export type PasswordField = FormItemRef
	export type TextArea = FormItemRef
	export type Checkbox = FormItemRef
	export type DatePicker = FormItemRef
	export type Dropdown = FormItemRef
	export type TagPicker = FormItemRef
	export type FilePicker = FormItemRef;

	export type ItemReference = FormItemRef;

	export type ItemProps<T extends Value> = FormItemProps<T>;
	
	export type Value = string | number | boolean | string[] | number[] | Date | null;

	export type Values = {
		[itemId: string]: Value
	};
}

const FormRoot: React.FC<Form.Props> = ({ enableDrafts = false, actions, children, isLoading = false, navigationTitle, searchBarAccessory }) => {
	return (
		<form 
		enableDrafts={enableDrafts}
		isLoading={isLoading}
		navigationTitle={navigationTitle}
		>
			{searchBarAccessory}
			{children}
			{actions}
		</form>
	)
}

interface WithFormRef<T> {
	ref?: Ref<T>;
};

interface TextFieldProps extends FormItemProps<string>, WithFormRef<Form.TextField> {
};

const TextField: React.FC<TextFieldProps> = ({ ref , ...props }) => {
	useImperativeFormHandle(ref);

	return <text-field {...props} />
}

interface PasswordFieldProps extends FormItemProps<string>, WithFormRef<Form.PasswordField> {
};

const PasswordField: React.FC<PasswordFieldProps> = ({ ...props }) => {
	return <password-field {...props} />
}

interface DatePickerProps extends FormItemProps<Date | null>, WithFormRef<Form.DatePicker> {
};

const DatePicker: React.FC<DatePickerProps> = ({ ...props }) => {
	return <date-picker-field {...props} />
}

export const Form = Object.assign(FormRoot, {
	TextField,
	PasswordField,
	DatePicker,
	Separator: () => <separator />
})
