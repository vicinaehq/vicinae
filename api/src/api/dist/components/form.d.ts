import { Ref } from 'react';
type FormProps = {
    actions: React.ReactNode;
    children: React.ReactNode;
    enableDrafts: boolean;
    isLoading: boolean;
    navigationTitle: string;
    searchBarAccessory: React.ReactNode;
};
export type FormItemRef = {
    focus: () => void;
    reset: () => void;
};
interface FormItemProps<T extends Form.Value> {
    id: string;
    ttile?: string;
    info?: string;
    error?: string;
    storeValue?: boolean;
    autoFocus?: boolean;
    value?: T;
    defaultValue?: T;
    onChange?: (newValue: T) => void;
    onBlur?: (event: FormEvent<T>) => void;
    onFocus?: (event: FormEvent<T>) => void;
}
interface FormEvent<T extends Form.Value> {
    target: {
        id: string;
        value?: T;
    };
    type: FormEventType;
}
type FormEventType = 'focus' | 'blur';
export declare namespace Form {
    type Props = FormProps;
    type TextField = FormItemRef;
    type PasswordField = FormItemRef;
    type TextArea = FormItemRef;
    type Checkbox = FormItemRef;
    type DatePicker = FormItemRef;
    type Dropdown = FormItemRef;
    type TagPicker = FormItemRef;
    type FilePicker = FormItemRef;
    type ItemReference = FormItemRef;
    type ItemProps<T extends Value> = FormItemProps<T>;
    type Value = string | number | boolean | string[] | number[] | Date | null;
    type Values = {
        [itemId: string]: Value;
    };
}
interface WithFormRef<T> {
    ref: Ref<T>;
}
interface TextFieldProps extends FormItemProps<string>, WithFormRef<Form.TextField> {
}
interface PasswordFieldProps extends FormItemProps<string>, WithFormRef<Form.PasswordField> {
}
export declare const Form: import("react").FC<FormProps> & {
    TextField: import("react").FC<TextFieldProps>;
    PasswordField: import("react").FC<PasswordFieldProps>;
};
export {};
//# sourceMappingURL=form.d.ts.map