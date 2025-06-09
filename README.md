# DIY Reflow Oven
## Objective
We are going to be building a lot of SMD PCBs in the future and so we would want to invest in a reflow oven. But the current consumer reflow ovens are very expensive and also require a lot of mods to get them working well. For these reasons, we decided to build our own reflow oven. 


## Requirements
- It should be able to fit a medium-sized board.
- It should look aesthetically pleasing (Wires should not be dangling around.)

## Components
| Component                                 | Link                                                                                                                      | Cost / Status     |
|-------------------------------------------|---------------------------------------------------------------------------------------------------------------------------|-------------------|
| Hamilton Beach toaster oven               | [Link](https://www.amazon.com/gp/product/B08CW95ZFR/ref=ewc_pr_img_1?smid=ATVPDKIKX0DER&psc=1)                            | $63.74            |
| Panasonic Solid State relay (AQA211VL)    | [AQA211VL](https://www.digikey.com/en/products/detail/panasonic-electric-works/AQA211VL/2364682)                           | $20.44            |
| Thermocouple IC (MAX31855KASA+T)          | [MAX31855KASA+T](https://www.digikey.com/en/products/detail/analog-devices-inc-maxim-integrated/MAX31855KASA-T/2591213)   | $8.39             |
| K-Type thermocouple wire (240-080)        | [240-080](https://www.digikey.com/en/products/detail/digilent-inc/240-080/5418219)                                         | $9.99             |
| RTD PT100 Sensor                          | [Link](https://www.amazon.com/T-PRO-Bonded-PT100-Sensor-3-3Ft/dp/B08LM65RR9/ref=sr_1_1_sspa?dib=eyJ2IjoiMSJ9.ho2rK_0WDDtzblfta3BW38M7qYvQeWUQCptRPbSS0BLeCsDtmtxTRNIGxjzQsyyU5wBJDnWwgiXgdA0Wa3jods1JWFmNPTgaX-OKxc_kS5rdJuIUgGGF4iHpwLhBHd4v9ZD8DF-NOKP-BuiiUtAb_fOJcZcli_CCJBb9d0RBt79JrkZgC5OPRpzAKGa2WIGDiQFRGdwM_pzMzyg8votbx8N-DyOa5gWSov0g3i_Jxe4.gcv0qolQDvdorbPu7aZP2Ob7Y0XtCTAz7jK61YfO9O4&dib_tag=se&keywords=pt100+rtd&qid=1749510461&sr=8-1-spons&sp_csd=d2lkZ2V0TmFtZT1zcF9hdGY&psc=1)                                                                                                                         | *Link/15.98  |
|MAX31865 RTD Temperature Thermocouple      | [Link](https://www.amazon.com/HiLetgo-MAX31865-Temperature-Thermocouple-Amplifier/dp/B071DVVZHC/ref=pd_bxgy_d_sccl_2/135-4655259-4365117?pd_rd_w=0znua&content-id=amzn1.sym.de9a1315-b9df-4c24-863c-7afcb2e4cc0a&pf_rd_p=de9a1315-b9df-4c24-863c-7afcb2e4cc0a&pf_rd_r=BFC7F3X7E3RN3DVGXN7T&pd_rd_wg=TNM6I&pd_rd_r=45f8c50e-10f0-4ade-a76b-58bd8f712acb&pd_rd_i=B071DVVZHC&psc=1) |13.49        |
| Ceramic Blanket                           | [Link](https://www.aliexpress.us/item/3256802930270704.html?spm=a2g0o.cart.0.0.20b138da8MZPSK&mp=1&gatewayAdapt=glo2usa)   | $9.20             |
| Aluminum reflective tape                  | —                                                                                                                         | In stock          |
| 240×320 SPI Display                       | —                                                                                                                         | In stock       |
| ESP32                                     | —                                                                                                                         | In stock        |
| 12V 1A Power Supply                       | [Link](https://www.aliexpress.us/item/3256802541723599.html?spm=a2g0o.cart.0.0.390f38daALcrNV&mp=1&gatewayAdapt=glo2usa)   | $5.10             |
| Cooling Fan                               | —                                                                                                                         | In stock        |
| Green LED                                 | [LED](https://www.aliexpress.com/item/2251832715504953.html?spm=a2g0o.cart.0.0.76ee38daP2Q4bT&mp=1)                            | $3.04             |
| Red LED                                   | [LED](https://www.aliexpress.us/item/2251832715504953.html?spm=a2g0o.cart.0.0.76ee38daP2Q4bT&mp=1&gatewayAdapt=glo2usa)     | $2.61             |
| Buzzer                                    | —                                                                                                                         | In stock        |
| Switch                                    | —                                                                                                                         | In stock       |
| Relay                                     | —                                                                                                                         | In stock        |
| Terminal Blocks                           | —                                                                                                                         | In stock        |
| USB-C to Micro USB cable                  | *Link TBD*                                                                                                                | —                 |


