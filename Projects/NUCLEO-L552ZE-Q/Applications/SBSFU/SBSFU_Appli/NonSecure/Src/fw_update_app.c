/**
  ******************************************************************************
  * @file    fw_update_app.c
  * @author  MCD Application Team
  * @brief   Firmware Update module.
  *          This file provides set of firmware functions to manage Firmware
  *          Update functionalities.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "stm32l5xx_hal.h"
#include "com.h"
#include "common.h"
#include "flash_if.h"
#include "ymodem.h"
#include "fw_update_app.h"
#include "flash_layout.h"


#include "string.h"

/** @addtogroup USER_APP User App Example
  * @{
  */

/** @addtogroup  FW_UPDATE Firmware Update Example
  * @{
  */
/** @defgroup  FW_UPDATE_Private_Variables Private Variables
  * @{
  */
static uint32_t m_uFileSizeYmodem = 0U;  /* !< Ymodem File size*/
static uint32_t m_uNbrBlocksYmodem = 0U; /* !< Ymodem Number of blocks*/
static uint32_t m_uPacketsReceived = 0U; /* !< Ymodem packets received*/

/**
  * @}
  */

/** @defgroup  FW_UPDATE_Private_Functions Private Functions
  * @{
  */
static void FW_UPDATE_PrintWelcome(void);
static HAL_StatusTypeDef FW_UPDATE_DownloadNewFirmware(SFU_FwImageFlashTypeDef *pFwImageDwlArea);
static HAL_StatusTypeDef FW_UPDATE_SECURE_IMAGE(void);
static HAL_StatusTypeDef FW_UPDATE_NONSECURE_IMAGE(void);
/**
  * @}
  */
/** @defgroup  FW_UPDATE_Exported_Functions Exported Functions
  * @{
  */

/** @defgroup  FW_UPDATE_Control_Functions Control Functions
  * @{
   */
void FW_UPDATE_Run(void)
{
  uint8_t key = 0U;
  uint8_t exit = 0U;

  /* Print Firmware Update welcome message */
  FW_UPDATE_PrintWelcome();

  while (exit == 0U)
  {
    key = 0U;

    /* Clean the input path */
    COM_Flush();

    /* Receive key */
    if (COM_Receive(&key, 1U, RX_TIMEOUT) == HAL_OK)
    {
      switch (key)
      {
        case '1' :
          FW_UPDATE_SECURE_IMAGE();
          break;
        case '2' :
          FW_UPDATE_NONSECURE_IMAGE();
          break;
        case '3' :
          printf("  -- Install image : reboot\r\n\n");
          NVIC_SystemReset();
          break;
        case 'x' :
          exit = 1U;
          break;
        default:
          printf("Invalid Number !\r");
          break;
      }
      /* Print Main Menu message */
      FW_UPDATE_PrintWelcome();
    }

  }
}
/**
  * @brief  Run FW Update process.
  * @param  None
  * @retval HAL Status.
  */
static HAL_StatusTypeDef FW_UPDATE_SECURE_IMAGE(void)
{
  HAL_StatusTypeDef ret = HAL_ERROR;
  SFU_FwImageFlashTypeDef fw_image_dwl_area;

  /* Print Firmware Update welcome message */
  printf("Download Secure Image\r\n");

  /* Get Info about the download area */
  fw_image_dwl_area.DownloadAddr =  FLASH_AREA_2_OFFSET + FLASH_BASE;
  fw_image_dwl_area.MaxSizeInBytes = FLASH_S_PARTITION_SIZE;
  fw_image_dwl_area.ImageOffsetInBytes = 0x0;


  /* Download new firmware image*/
  ret = FW_UPDATE_DownloadNewFirmware(&fw_image_dwl_area);

  if (HAL_OK == ret)
  {
    printf("  -- Secure Image correctly downloaded \r\n\n");
    HAL_Delay(1000U);
  }

  return ret;
}
/**
  * @brief  Run FW Update process.
  * @param  None
  * @retval HAL Status.
  */
static HAL_StatusTypeDef FW_UPDATE_NONSECURE_IMAGE(void)
{
  HAL_StatusTypeDef ret = HAL_ERROR;
  SFU_FwImageFlashTypeDef fw_image_dwl_area;

  /* Print Firmware Update welcome message */
  printf("Download NonSecure Image\r\n");

  /* Get Info about the download area */
  fw_image_dwl_area.DownloadAddr =  FLASH_AREA_3_OFFSET + FLASH_BASE;
  fw_image_dwl_area.MaxSizeInBytes = FLASH_NS_PARTITION_SIZE;
  fw_image_dwl_area.ImageOffsetInBytes = 0x0;


  /* Download new firmware image*/
  ret = FW_UPDATE_DownloadNewFirmware(&fw_image_dwl_area);

  if (HAL_OK == ret)
  {
    printf("  -- NonSecure Image correctly downloaded \r\n\n");
    HAL_Delay(1000U);
  }

  return ret;
}

/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup  FW_UPDATE_Private_Functions
  * @{
  */

/**
  * @brief  Display the FW_UPDATE Main Menu choices on HyperTerminal
  * @param  None.
  * @retval None.
  */
static void FW_UPDATE_PrintWelcome(void)
{
  printf("\r\n================ New Fw Download =========================\r\n\n");
  printf("  Download Secure Image --------------------------------- 1\r\n\n");
  printf("  Download NonSecure Image------------------------------- 2\r\n\n");
  printf("  Reset to trigger Installation-------------------------- 3\r\n\n");
  printf("  Exit New FW Download Menu ----------------------------- x\r\n\n");
}
/**
  * @brief Download a new Firmware from the host.
  * @retval HAL status
  */
static HAL_StatusTypeDef FW_UPDATE_DownloadNewFirmware(SFU_FwImageFlashTypeDef *pFwImageDwlArea)
{
  HAL_StatusTypeDef ret = HAL_ERROR;
  COM_StatusTypeDef e_result;
  uint32_t u_fw_size = pFwImageDwlArea->MaxSizeInBytes ;

  printf("  -- Send Firmware \r\n\n");
  /* Download binary */
  printf("  -- -- File> Transfer> YMODEM> Send \t\n");

  /*Init of Ymodem*/
  Ymodem_Init();

  /*Receive through Ymodem*/
  e_result = Ymodem_Receive(&u_fw_size, pFwImageDwlArea->DownloadAddr);
  printf("\r\n\n");

  if ((e_result == COM_OK))
  {
    printf("  -- -- Programming Completed Successfully!\r\n\n");
#if defined(__ARMCC_VERSION)
    printf("  -- -- Bytes: %u\r\n\n", u_fw_size);
#else
    printf("  -- -- Bytes: %lu\r\n\n", u_fw_size);
#endif /*  __ARMCC_VERSION */
    ret = HAL_OK;
  }
  else if (e_result == COM_ABORT)
  {
    printf("  -- -- !!Aborted by user!!\r\n\n");
    COM_Flush();
    ret = HAL_ERROR;
  }
  else
  {
    printf("  -- -- !!Error during file download!!\r\n\n");
    ret = HAL_ERROR;
    HAL_Delay(500U);
    COM_Flush();
  }

  return ret;
}


/**
  * @}
  */

/** @defgroup FW_UPDATE_Callback_Functions Callback Functions
  * @{
  */

/**
  * @brief  Ymodem Header Packet Transfer completed callback.
  * @param  uFileSize Dimension of the file that will be received (Bytes).
  * @retval None
  */
HAL_StatusTypeDef Ymodem_HeaderPktRxCpltCallback(uint32_t uFlashDestination, uint32_t uFileSize)
{
  HAL_StatusTypeDef ret = HAL_ERROR;
  /*Reset of the ymodem variables */
  m_uFileSizeYmodem = 0U;
  m_uPacketsReceived = 0U;
  m_uNbrBlocksYmodem = 0U;

  /*Filesize information is stored*/
  m_uFileSizeYmodem = uFileSize;

  /* compute the number of 1K blocks */
  m_uNbrBlocksYmodem = (m_uFileSizeYmodem + (PACKET_1K_SIZE - 1U)) / PACKET_1K_SIZE;
  if ((uFlashDestination >= 0x8000000) && (uFlashDestination < 0x8080000))
  {
    ret = FLASH_If_Erase_Size((void *)(uFlashDestination), uFileSize);
  }
  else while (1);
  /* NOTE : delay inserted for Ymodem protocol*/
  HAL_Delay(1000);
  return ret;
}

extern uint32_t total_size_received;
/**
  * @brief  Ymodem Data Packet Transfer completed callback.
  * @param  pData Pointer to the buffer.
  * @param  uSize Packet dimension (Bytes).
  * @retval None
  */
HAL_StatusTypeDef Ymodem_DataPktRxCpltCallback(uint8_t *pData, uint32_t uFlashDestination, uint32_t uSize)
{
  HAL_StatusTypeDef e_ret_status = HAL_ERROR;

  m_uPacketsReceived++;

  /*Increase the number of received packets*/
  if (m_uPacketsReceived == m_uNbrBlocksYmodem) /*Last Packet*/
  {
    /*Extracting actual payload from last packet*/
    if (0 == (m_uFileSizeYmodem % PACKET_1K_SIZE))
    {
      /* The last packet must be fully considered */
      uSize = PACKET_1K_SIZE;
    }
    else
    {
      /* The last packet is not full, drop the extra bytes */
      uSize = m_uFileSizeYmodem - ((uint32_t)(m_uFileSizeYmodem / PACKET_1K_SIZE) * PACKET_1K_SIZE);
    }

    m_uPacketsReceived = 0U;
  }
  if ((uFlashDestination >= 0x8000000) && (uFlashDestination < 0x8080000))
  {
    /*Adjust dimension to 64-bit length */
    if (uSize % FLASH_IF_MIN_WRITE_LEN != 0U)
    {
      uSize += (FLASH_IF_MIN_WRITE_LEN - (uSize % FLASH_IF_MIN_WRITE_LEN));
    }

    /* Write Data in Flash - size has to be 64-bit aligned */
    if (FLASH_If_Write((void *)uFlashDestination, pData, uSize) == HAL_OK)
    {
      e_ret_status = HAL_OK;

    }
    else
    {
      e_ret_status = HAL_ERROR;
    }
  }
  else
  {
    /*  ram buffer  */
    memcpy((void *)uFlashDestination, pData, uSize);
    e_ret_status = HAL_OK;
  }
  if (e_ret_status == HAL_ERROR)
  {

    /*Reset of the ymodem variables */
    m_uFileSizeYmodem = 0U;
    m_uPacketsReceived = 0U;
    m_uNbrBlocksYmodem = 0U;
  }
  return e_ret_status;
}


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/