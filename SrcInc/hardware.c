

void enable_24V_intern()
{
	HAL_GPIO_WritePin(GPIOB, V24ON1_Pin, GPIO_PIN_SET); // 24VINTERN
}
void enable_24V_extern()
{
	HAL_GPIO_WritePin(GPIOB, V24ON2_Pin, GPIO_PIN_SET); // 24VOUT
}
void disable_24V_intern()
{
	HAL_GPIO_WritePin(GPIOB, V24ON1_Pin, GPIO_PIN_RESET); // 24VINTERN
}
void disable_24V_extern()
{
	HAL_GPIO_WritePin(GPIOB, V24ON2_Pin, GPIO_PIN_RESET); // 24VOUT
}
void power_24v_off()
{
	disable_24V_intern();
	disable_24V_extern();
}
void power_24v_on()
{
	enable_24V_intern();
	enable_24V_extern();
}